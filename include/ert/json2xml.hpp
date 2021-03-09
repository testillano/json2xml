/*
 ________________________________________________
|     _                 ___                 _   |
|    (_)               |__ \               | |  |
|     _ ___  ___  _ __    ) |_  ___ __ ___ | |  |
|    | / __|/ _ \| '_ \  / /\ \/ / '_ ` _ \| |  |  Json-to-xml C++ only-header library
|    | \__ \ (_) | | | |/ /_ >  <| | | | | | |  |  Version 1.0.0
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
#include <iomanip>
#include <sstream>
#include <stack>

// Nlohmann (https://github.com/nlohmann/json)
#include <nlohmann/json.hpp>

namespace ert {

class JsonSaxConsumer : public nlohmann::json::json_sax_t
{
    // private members
    int tab_spaces_;
    char attr_prefix_;
    bool has_attributes_;
    int indent_;
    std::stringstream result_;
    std::stringstream current_object_;
    std::stack<std::string> nodes_stack_;
    std::string key_;

    // private functions
    const std::string & get_top() {
        return nodes_stack_.top();
    }


public:

    /**
    * Default constructor
    *
    * @param tabSpaces Indentation spaces for xml representation, 4 by default. Must be >= 1 or 4 will be asigned.
    * @param attrPrefix You could set any other value for attribute prefix, '@' by default.
    */
    JsonSaxConsumer(int tabSpaces = 4, char attrPrefix = '@'):
        tab_spaces_(tabSpaces>=1 ? tabSpaces:4),
        attr_prefix_(attrPrefix),
        has_attributes_(false),
        indent_(-tab_spaces_) {};

    /**
    * Returns the resulting xml stringstream representation
    */
    const std::stringstream & getXmlStream() const {
        return result_;
    }

    /**
    * Returns the resulting xml string representation
    */
    std::string getXmlString() const {
        return result_.str();
    }

    // nlohmann sax virtual methods
    bool null() override
    {
        current_object_ << "<null>";
        has_attributes_ = true;
        return true;
    }

    bool boolean(bool val) override
    {
        current_object_ << std::quoted(val ? "true" : "false");
        has_attributes_ = true;
        return true;
    }

    bool number_integer(number_integer_t val) override
    {
        current_object_ << std::quoted(std::to_string(val));
        has_attributes_ = true;
        return true;
    }

    bool number_unsigned(number_unsigned_t val) override
    {
        current_object_ << std::quoted(std::to_string(val));
        has_attributes_ = true;
        return true;
    }

    bool number_float(number_float_t val, const string_t& s) override
    {
        current_object_ << std::quoted(s);
        has_attributes_ = true;
        return true;
    }

    bool string(string_t& val) override
    {
        current_object_ << std::quoted(val);
        has_attributes_ = true;
        return true;
    }
    bool binary(binary_t& val) override
    {
        current_object_ << std::quoted("binary"); // TODO
        has_attributes_ = true;
        return true;
    }

    bool start_object(std::size_t elements) override
    {
        if (key_ == "") return true; // global object condition (first start object)
        nodes_stack_.push(key_); // push on starts
        indent_ += tab_spaces_; // increase indentation on object start

        if (indent_ != 0) {
            // New object when previous hadn't attributes:
            if (current_object_.str().empty() && !has_attributes_) result_ <<">";
            result_ << "\n";
        }
        result_ << std::string(indent_, ' ') << "<" << get_top();

        has_attributes_ = false;
        return true;
    }

    bool end_object() override
    {
        if (indent_ < 0) return true;
        if (current_object_.str().empty()) {
            result_ << "\n" << std::string(indent_, ' ') << "</" << get_top() << ">";
        }
        else {
            result_ << current_object_.str() << "/>";
            current_object_.str("");
        }

        nodes_stack_.pop(); // pop on ends
        indent_ -= tab_spaces_; // decrease indentation on object end
        return true;
    }

    bool start_array(std::size_t elements) override
    {
        nodes_stack_.push(key_); // push on starts

        result_ << current_object_.str();

        current_object_.str("");
        has_attributes_ = false;

        return true;
    }

    bool end_array() override
    {
        nodes_stack_.pop(); // pop on ends
        return true;
    }

    bool key(string_t& val) override
    {
        if (val[0] != attr_prefix_) {
            key_ = val;
        }
        else {
            current_object_ << " " << val.substr(1) << "=";
        }
        return true;
    }

    bool parse_error(std::size_t position, const std::string& last_token, const nlohmann::json::exception& ex) override
    {
        result_ << std::string(ex.what());
        return false;
    }
};

}

#endif

