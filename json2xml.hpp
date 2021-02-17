// Standard
#include <iomanip>
#include <sstream>
#include <stack>

// Nlohmann (https://github.com/nlohmann/json)
#include "nlohmann/json.hpp"


#define XML_INDENTATION_SPACES 4

class JsonSaxConsumer : public nlohmann::json::json_sax_t
{
    // private members
    char attr_prefix_;
    int indent_;
    std::stringstream result_;
    std::stringstream current_object_;
    std::stack<std::string> nodes_stack_;
    std::string key_;
    bool has_attributes_;

    // private functions
    const std::string & get_top() {
        return nodes_stack_.top();
    }


public:

    /**
    * Default constructor
    *
    * @param attrPrefix You could set any other value for attribute prefix, '@' by default
    */
    JsonSaxConsumer(char attrPrefix = '@') : attr_prefix_(attrPrefix),
        has_attributes_(false),
        indent_(-XML_INDENTATION_SPACES) {};

    /**
    * Returns the xml stringstream representation
    */
    const std::stringstream & getResult() const {
        return result_;
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

    bool start_object(std::size_t elements) override
    {
        if (key_ == "") return true; // global object condition (first start object)
        nodes_stack_.push(key_); // push on starts
        indent_ += XML_INDENTATION_SPACES; // increase indentation on object start

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
        indent_ -= XML_INDENTATION_SPACES; // decrease indentation on object end
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

