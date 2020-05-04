#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <stack>

#include "nlohmann/json.hpp"

#define XML_INDENTATION_SPACES 4

class JsonSaxConsumer : public nlohmann::json::json_sax_t
{
  int indent_;
  bool started_;
  std::stringstream result_;
  std::stringstream current_object_;
  std::stack<std::string> nodes_stack_;
  std::string key_;

  public:

  JsonSaxConsumer() : started_(false), indent_(-XML_INDENTATION_SPACES) {}

  const std::stringstream & getResult() const { return result_; }

  bool null() override
  {
    current_object_ << "<null>";
    return true;
  }

  bool boolean(bool val) override
  {
    current_object_ << std::quoted(val ? "true" : "false");
    return true;
  }

  bool number_integer(number_integer_t val) override
  {
    current_object_ << std::quoted(std::to_string(val));
    return true;
  }

  bool number_unsigned(number_unsigned_t val) override
  {
    current_object_ << std::quoted(std::to_string(val));
    return true;
  }

  bool number_float(number_float_t val, const string_t& s) override
  {
    current_object_ << std::quoted(s);
    return true;
  }

  bool string(string_t& val) override
  {
    current_object_ << std::quoted(val);
    return true;
  }

  bool start_object(std::size_t elements) override
  {
    nodes_stack_.push(key_);
    if (!started_) { started_ = true ; return true; }
    indent_ += XML_INDENTATION_SPACES;
    result_ << std::string(indent_, ' ') << "<" << nodes_stack_.top();
    return true;
  }

  bool end_object() override
  {
    std::string close = "/>";
    if (current_object_.str().empty()) close = "";
    result_ << current_object_.str() << close;
    if (indent_ < 0) return true;
    indent_ -= XML_INDENTATION_SPACES;
    if (close == "") result_ << "</" << nodes_stack_.top() << ">";
    result_ << "\n";
    current_object_.str("");
    nodes_stack_.pop();
    return true;
  }

  bool start_array(std::size_t elements) override
  {
    nodes_stack_.push(key_);
    result_ << current_object_.str() << ">\n";
    current_object_.str("");
    return true;
  }

  bool end_array() override
  {
    result_ << std::string(indent_, ' ');
    nodes_stack_.pop();
    return true;
  }

  bool key(string_t& val) override
  {
    if (val[0] != '@') {
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

int main(int argc, char *argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: json2xml <json file|test>" << std::endl;
    return 1;
  }

  std::string argument = argv[1];
  std::string jsonStr;

  if (argument != "test") {
    std::ifstream ifs(argument);
    std::stringstream ss;
    ss << ifs.rdbuf();
    jsonStr = ss.str();
    if (jsonStr == "") {
      std::cerr << "Cannot read file provided !" << std::endl;
      return 1;
    }
  }
  else {
    jsonStr = R"(
{
    "node": {
        "@attrib1": "1",
        "@attrib2": "2",
        "child": [
            {
                "@child-attrib1": "child1 attribute 1",
                "@child-attrib2": "child1 attribute 2"
            },
            {
                "@child-attrib1": "child2 attribute 1",
                "@child-attrib2": "child2 attribute 2"
            },
            {
                "@grandchilds": "little ones",
                "grandchild": [
                    {
                        "@grandchild-attrib1": "grandchild1 attribute 1"
                    },
                    {
                        "@grandchild-attrib1": "grandchild2 attribute 1"
                    }
                ]
            },
            {
                "@child-attrib1": "child3 attribute 1",
                "@child-attrib2": "child3 attribute 2"
            }
        ]
    }
}
    )";
  }

  // Consume json to build xml:
  JsonSaxConsumer consumer;
  bool success = nlohmann::json::sax_parse(jsonStr, &consumer);

  if (!success)
    std::cerr << "Conversion error !" << std::endl;

  // output xml
  std::cout << consumer.getResult().str();

  // exit status
  return success ? 0:1;
}

