#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <stack>

#include "nlohmann/json.hpp"

#define XML_INDENTATION_SPACES 4

class JsonSaxConsumer : public nlohmann::json::json_sax_t
{
  char attr_prefix_;
  int indent_;
  std::stringstream result_;
  std::stringstream current_object_;
  std::stack<std::string> nodes_stack_;
  std::string key_;
  bool has_attributes_;

  const std::string & get_top() { return nodes_stack_.top(); }

  /*
  void log(const std::string &literal) {

    std::cout << "========================" << std::endl;
    std::cout << literal << std::endl;
    std::cout << "========================" << std::endl;
    std::cout << "key_ " << key_ << std::endl;
    std::cout << "indent_ " << indent_ << std::endl;
    std::cout << "current_object_ " << current_object_.str() << std::endl;
    std::cout << "nodes_stack_ SIZE " << nodes_stack_.size() << std::endl;
    std::cout << "nodes_stack_ TOP " << nodes_stack_.top() << std::endl;
    std::cout << "key_ " << key_ << std::endl;
    std::cout << "------------------------" << std::endl;
    std::cout << result_.str() << std::endl;
    std::cout << "------------------------" << std::endl;
  }
  */

  public:

  JsonSaxConsumer(char attrPrefix = '@') : attr_prefix_(attrPrefix),
                                           has_attributes_(false),
                                           indent_(-XML_INDENTATION_SPACES) {};

  const std::stringstream & getResult() const { return result_; }

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

int main(int argc, char *argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: json2xml <json file|test: harcoded test>" << std::endl;
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
    "grandfather": {
        "father": {
            "child": [
                {
                    "@name": "Jose",
                    "@bestFriend": "Maria"
                },
                {
                    "@name": "Maria",
                    "@bestFriend": "Jose"
                }
            ],
            "student": {
                "@favoriteBook": "The Gods Themselves",
                "@age": "21"
            }
        }
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

