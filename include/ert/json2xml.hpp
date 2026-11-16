/*
 ________________________________________________
|     _                 ___                 _   |
|    (_)               |__ \               | |  |
|     _ ___  ___  _ __    ) |_  ___ __ ___ | |  |
|    | / __|/ _ \| '_ \  / /\ \/ / '_ ` _ \| |  |  Json-to-xml C++ only-header library
|    | \__ \ (_) | | | |/ /_ >  <| | | | | | |  |  Version 1.1.0
|    | |___/\___/|_| |_|____/_/\_\_| |_| |_|_|  |  https://github.com/testillano/json2xml
|   _/ |                                        |
|  |__/                                         |
|_______________________________________________|

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2020 Eduardo Ramos

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ERT_JSONSAXCONSUMER_H_
#define ERT_JSONSAXCONSUMER_H_

// Standard
#include <cstddef>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// Nlohmann (https://github.com/nlohmann/json)
#include <nlohmann/json.hpp>

namespace ert {

/**
 * Json SAX consumer building the XML representation of the parsed json document.
 *
 * The json layout is the one of python xmltodict, restricted to elements and
 * attributes:
 *
 * - The top level json value must be an object: each of its keys is a root
 *   element (normally one; several give an XML fragment).
 * - A key with an object value is an element: its prefixed keys ('@' by default)
 *   with scalar values (string, number, boolean) are the attributes, and the
 *   other keys the child elements. Attributes are written first, wherever they
 *   are in the object.
 * - A key with an array value is a repeated element: one per item (objects, or
 *   nulls for empty elements).
 * - A key with a null value, or an empty object, is an empty element.
 *
 * The conversion fails (sax_parse() returns false and getError() tells why) on
 * json parse errors and on what is not supported: text content (an element key
 * with a scalar value, or an array of scalars), nested arrays, attributes at top
 * level, with object/array/null values or repeated, invalid XML names, and
 * characters not allowed in XML 1.0. Attribute values are escaped ('&', '<',
 * '>', '"', tab, line feed and carriage return).
 *
 * Usage:
 * @code
 * ert::JsonSaxConsumer consumer(2);
 * if (nlohmann::json::sax_parse(jsonString, &consumer)) xml = consumer.getXmlString();
 * else error = consumer.getError();
 * @endcode
 *
 * A consumer can be reused: every parse starts from scratch.
 */
class JsonSaxConsumer : public nlohmann::json::json_sax_t
{
    enum class FrameType { Document, Element, Array };

    // Open json container
    struct Frame {
        FrameType type;
        std::string name;                    // element name (Element), repeated element name (Array)
        std::size_t depth;                   // indentation level of the element(s) (Element, Array)
        std::string attributes;              // ' name="value"' list (Element)
        std::set<std::string> attributeNames; // to reject repeated attributes (Element)
        std::string children;                // rendered child elements (Document, Element)

        Frame(FrameType t, std::string n, std::size_t d) : type(t), name(std::move(n)), depth(d) {}
    };

    // private members
    int tab_spaces_;
    char attr_prefix_;
    std::stringstream result_;
    std::string error_;
    std::vector<Frame> frames_;
    std::string key_;              // last key: element name, or attribute name without prefix
    bool key_is_attribute_ = false;

    // private functions
    bool fail(const std::string& message) {
        error_ = message;
        result_.str("");
        frames_.clear();
        return false;
    }

    bool failNotObject() {
        return fail("a json object is expected at top level");
    }

    std::string indentation(std::size_t depth) const {
        return std::string(depth * tab_spaces_, ' ');
    }

    // Indentation level of the elements created within a frame
    static std::size_t childDepth(const Frame& frame) {
        switch (frame.type) {
        case FrameType::Document:
            return 0;
        case FrameType::Element:
            return frame.depth + 1;
        default:
            return frame.depth; // array items
        }
    }

    // Frame where the elements created now are rendered (array items go to the array owner)
    Frame& owner() {
        return frames_.back().type == FrameType::Array ? frames_[frames_.size() - 2] : frames_.back();
    }

    // Json location of the current value, for error messages (e.g. '/a/b/@c')
    std::string location() const {
        std::string result;
        for (const auto& frame : frames_) {
            if (frame.type == FrameType::Element) result += "/" + frame.name;
        }
        const Frame& top = frames_.back();
        if (top.type == FrameType::Array) {
            result += "/" + top.name + "[]";
        }
        else {
            result += "/" + (key_is_attribute_ ? std::string(1, attr_prefix_) : std::string()) + key_;
        }
        return result;
    }

    // Name and indentation of the element created by the current value (keyed or array item)
    std::pair<std::string, std::size_t> newElement() const {
        const Frame& top = frames_.back();
        return {top.type == FrameType::Array ? top.name : key_, childDepth(top)};
    }

    void appendElement(Frame& target, const std::string& name, std::size_t depth,
                       const std::string& attributes, const std::string& children) {
        target.children += indentation(depth) + "<" + name + attributes;
        if (children.empty()) {
            target.children += "/>\n";
        }
        else {
            target.children += ">\n" + children + indentation(depth) + "</" + name + ">\n";
        }
    }

    static bool isNameStartChar(unsigned char c) {
        // ASCII letters, '_', ':' and any non-ASCII (UTF-8) byte
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == ':' || c >= 0x80;
    }

    static bool isNameChar(unsigned char c) {
        return isNameStartChar(c) || (c >= '0' && c <= '9') || c == '-' || c == '.';
    }

    static bool isValidName(const std::string& name) {
        if (name.empty() || !isNameStartChar(static_cast<unsigned char>(name[0]))) return false;
        for (char c : name) {
            if (!isNameChar(static_cast<unsigned char>(c))) return false;
        }
        return true;
    }

    // Escapes an attribute value; false for characters not allowed in XML 1.0
    static bool escape(const std::string& value, std::string& out) {
        for (char ch : value) {
            const unsigned char c = static_cast<unsigned char>(ch);
            switch (c) {
            case '&':
                out += "&amp;";
                break;
            case '<':
                out += "&lt;";
                break;
            case '>':
                out += "&gt;";
                break;
            case '"':
                out += "&quot;";
                break;
            case '\t':
                out += "&#9;";
                break;
            case '\n':
                out += "&#10;";
                break;
            case '\r':
                out += "&#13;";
                break;
            default:
                if (c < 0x20) return false;
                out += ch;
            }
        }
        return true;
    }

    // String, number and boolean values: only valid as attribute values
    bool scalar(const std::string& value, const std::string& type) {
        if (frames_.empty()) return failNotObject();
        Frame& top = frames_.back();
        if (top.type == FrameType::Array || !key_is_attribute_) {
            return fail(type + " value at '" + location() + "': text content is not supported (only attributes have scalar values)");
        }
        std::string escaped;
        if (!escape(value, escaped)) {
            return fail("value of attribute '" + location() + "' has a character not allowed in XML");
        }
        if (!top.attributeNames.insert(key_).second) {
            return fail("repeated attribute '" + location() + "'");
        }
        top.attributes += " " + key_ + "=\"" + escaped + "\"";
        return true;
    }

    bool failAttributeType(const std::string& type) {
        return fail(type + " value for attribute '" + location() + "': must be a string, number or boolean");
    }

public:

    /**
    * Default constructor
    *
    * @param tabSpaces Indentation spaces for xml representation, 4 by default. Must be >= 1 or 4 will be asigned.
    * @param attrPrefix You could set any other value for attribute prefix, '@' by default.
    */
    JsonSaxConsumer(int tabSpaces = 4, char attrPrefix = '@'):
        tab_spaces_(tabSpaces >= 1 ? tabSpaces : 4),
        attr_prefix_(attrPrefix) {}

    /**
    * Returns the resulting xml stringstream representation (empty on failure)
    */
    const std::stringstream & getXmlStream() const {
        return result_;
    }

    /**
    * Returns the resulting xml string representation (empty on failure)
    */
    std::string getXmlString() const {
        return result_.str();
    }

    /**
    * Returns the reason of the last failed conversion (empty on success)
    */
    const std::string & getError() const {
        return error_;
    }

    // nlohmann sax virtual methods
    bool null() override
    {
        if (frames_.empty()) return failNotObject();
        if (frames_.back().type != FrameType::Array && key_is_attribute_) return failAttributeType("null");
        const auto element = newElement();
        appendElement(owner(), element.first, element.second, "", ""); // empty element
        return true;
    }

    bool boolean(bool val) override
    {
        return scalar(val ? "true" : "false", "boolean");
    }

    bool number_integer(number_integer_t val) override
    {
        return scalar(std::to_string(val), "number");
    }

    bool number_unsigned(number_unsigned_t val) override
    {
        return scalar(std::to_string(val), "number");
    }

    bool number_float(number_float_t /*val*/, const string_t& s) override
    {
        return scalar(s, "number"); // as written in the json document
    }

    bool string(string_t& val) override
    {
        return scalar(val, "string");
    }

    bool binary(binary_t& /*val*/) override
    {
        if (frames_.empty()) return failNotObject();
        return fail("binary value at '" + location() + "' is not supported");
    }

    bool start_object(std::size_t /*elements*/) override
    {
        if (frames_.empty()) { // document start: a new conversion
            result_.str("");
            error_.clear();
            frames_.emplace_back(FrameType::Document, "", 0);
            return true;
        }
        if (frames_.back().type != FrameType::Array && key_is_attribute_) return failAttributeType("object");
        auto element = newElement();
        frames_.emplace_back(FrameType::Element, std::move(element.first), element.second);
        return true;
    }

    bool end_object() override
    {
        Frame frame = std::move(frames_.back());
        frames_.pop_back();

        if (frame.type == FrameType::Document) {
            if (frame.children.empty()) return fail("the json object has no element");
            frame.children.pop_back(); // no trailing new line
            result_ << frame.children;
            return true;
        }

        appendElement(owner(), frame.name, frame.depth, frame.attributes, frame.children);
        return true;
    }

    bool start_array(std::size_t /*elements*/) override
    {
        if (frames_.empty()) return failNotObject();
        if (frames_.back().type == FrameType::Array) {
            return fail("nested array at '" + location() + "' is not supported");
        }
        if (key_is_attribute_) return failAttributeType("array");
        auto element = newElement();
        frames_.emplace_back(FrameType::Array, std::move(element.first), element.second);
        return true;
    }

    bool end_array() override
    {
        frames_.pop_back();
        return true;
    }

    bool key(string_t& val) override
    {
        const bool attribute = !val.empty() && val[0] == attr_prefix_;
        key_ = attribute ? val.substr(1) : val;
        key_is_attribute_ = attribute;

        if (!isValidName(key_)) {
            return fail(std::string(attribute ? "attribute" : "element") + " name '" + val + "' at '" + location() + "' is not a valid XML name");
        }
        if (attribute && frames_.back().type == FrameType::Document) {
            return fail("attribute '" + val + "' at top level: attributes belong to an element");
        }
        return true;
    }

    bool parse_error(std::size_t /*position*/, const std::string& /*last_token*/, const nlohmann::json::exception& ex) override
    {
        return fail(ex.what());
    }
};

}

#endif
