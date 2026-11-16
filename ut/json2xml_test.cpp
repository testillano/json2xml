// Unit tests of the json to xml SAX consumer (include/ert/json2xml.hpp)
//
// They describe the current conversion. The DISABLED_ ones hold the expected
// result of known bugs (the current output is shown in their comment).

#include <gtest/gtest.h>

#include <ert/json2xml.hpp>

#include <string>

namespace {

struct Result {
    bool ok;
    std::string xml;
};

Result convert(const std::string& json, int tabSpaces = 2, char attrPrefix = '@') {
    ert::JsonSaxConsumer consumer(tabSpaces, attrPrefix);
    const bool ok = nlohmann::json::sax_parse(json, &consumer);
    return {ok, consumer.getXmlString()};
}

}  // namespace

////////////////
// Conversion //
////////////////

TEST(Json2xml, ReadmeExample) {
    const auto r = convert(R"({
      "grandfather": {
        "father": {
          "child": [
            {"@name": "Jose", "@bestFriend": "Maria"},
            {"@name": "Maria", "@bestFriend": "Jose"}
          ],
          "student": {"@favoriteBook": "The Gods Themselves", "@age": "21"}
        }
      }
    })");
    ASSERT_TRUE(r.ok);
    EXPECT_EQ(r.xml,
              "<grandfather>\n"
              "  <father>\n"
              "    <child name=\"Jose\" bestFriend=\"Maria\"/>\n"
              "    <child name=\"Maria\" bestFriend=\"Jose\"/>\n"
              "    <student favoriteBook=\"The Gods Themselves\" age=\"21\"/>\n"
              "  </father>\n"
              "</grandfather>");
}

TEST(Json2xml, AttributesOnly) {
    EXPECT_EQ(convert(R"({"a": {"@x": "1", "@y": "2"}})").xml, "<a x=\"1\" y=\"2\"/>");
}

TEST(Json2xml, NestedElements) {
    EXPECT_EQ(convert(R"({"a": {"b": {"c": {"@x": "1"}}}})").xml, "<a>\n  <b>\n    <c x=\"1\"/>\n  </b>\n</a>");
}

TEST(Json2xml, SiblingElements) {
    EXPECT_EQ(convert(R"({"a": {"b": {"@x": "1"}, "c": {"@y": "2"}}})").xml, "<a>\n  <b x=\"1\"/>\n  <c y=\"2\"/>\n</a>");
}

TEST(Json2xml, RepeatedElements) {
    EXPECT_EQ(convert(R"({"a": {"b": [{"@x": "1"}]}})").xml, "<a>\n  <b x=\"1\"/>\n</a>");
    EXPECT_EQ(convert(R"({"a": {"b": [{"@x": "1"}, {"@x": "2"}]}})").xml, "<a>\n  <b x=\"1\"/>\n  <b x=\"2\"/>\n</a>");
}

TEST(Json2xml, ArrayFollowedBySibling) {
    EXPECT_EQ(convert(R"({"r": {"c": [{"@i": "1"}, {"@i": "2"}], "d": {"@j": "3"}}})").xml,
              "<r>\n  <c i=\"1\"/>\n  <c i=\"2\"/>\n  <d j=\"3\"/>\n</r>");
}

TEST(Json2xml, ParentAttributesAndArray) {
    EXPECT_EQ(convert(R"({"node": {"@a": "1", "child": [{"@c": "1"}, {"@c": "2"}]}})").xml,
              "<node a=\"1\">\n  <child c=\"1\"/>\n  <child c=\"2\"/>\n</node>");
}

TEST(Json2xml, ScalarAttributeValues) {
    EXPECT_EQ(convert(R"({"a": {"@i": -3, "@u": 7, "@f": 1.50, "@e": 1e3, "@t": true, "@n": false}})").xml,
              "<a i=\"-3\" u=\"7\" f=\"1.50\" e=\"1e3\" t=\"true\" n=\"false\"/>");
}

TEST(Json2xml, AttributeValuesWithNoEscaping) {
    EXPECT_EQ(convert(R"({"a": {"@v": "it's a value", "@e": ""}})").xml, "<a v=\"it's a value\" e=\"\"/>");
    // json escapes decoded into UTF-8: "a\u00f1o" (n tilde), "\u20ac" (euro sign)
    EXPECT_EQ(convert(R"({"a\u00f1o": {"@price": "10 \u20ac"}})").xml, "<a\xc3\xb1o price=\"10 \xe2\x82\xac\"/>");
}

TEST(Json2xml, Indentation) {
    const std::string json = R"({"a": {"b": {"c": {"@x": "1"}}}})";
    EXPECT_EQ(convert(json, 4).xml, "<a>\n    <b>\n        <c x=\"1\"/>\n    </b>\n</a>");
    EXPECT_EQ(convert(json, 1).xml, "<a>\n <b>\n  <c x=\"1\"/>\n </b>\n</a>");
    EXPECT_EQ(convert(json, 0).xml, convert(json, 4).xml);  // invalid: 4 is used
    EXPECT_EQ(convert(json, -2).xml, convert(json, 4).xml);

    ert::JsonSaxConsumer defaults;
    ASSERT_TRUE(nlohmann::json::sax_parse(json, &defaults));
    EXPECT_EQ(defaults.getXmlString(), convert(json, 4).xml);
}

TEST(Json2xml, CustomAttributePrefix) {
    EXPECT_EQ(convert(R"({"a": {"b": {"-y": "2"}}})", 2, '-').xml, "<a>\n  <b y=\"2\"/>\n</a>");
}

TEST(Json2xml, XmlStreamMatchesString) {
    ert::JsonSaxConsumer consumer(2);
    ASSERT_TRUE(nlohmann::json::sax_parse(R"({"a": {"@x": "1"}})", &consumer));
    EXPECT_EQ(consumer.getXmlStream().str(), consumer.getXmlString());
}

TEST(Json2xml, JsonParseErrorFails) {
    EXPECT_FALSE(convert(R"({"a": )").ok);
}

////////////////
// Known bugs //
////////////////

// Current: "<a\n  <b x=\"1\"/>\n</a>" (the attribute goes to the child)
TEST(Json2xml, DISABLED_AttributesAndChildren) {
    EXPECT_EQ(convert(R"({"a": {"@x": "1", "b": {"@y": "2"}}})").xml, "<a x=\"1\">\n  <b y=\"2\"/>\n</a>");
}

// examples/test3.json (xfail in the component test): the item after a nested
// array takes the name of that array ('grandchild')
TEST(Json2xml, DISABLED_ArrayItemAfterNestedArray) {
    EXPECT_EQ(convert(R"({"r": {"c": [{"g": [{"@k": "a"}]}, {"@i": "2"}]}})").xml,
              "<r>\n  <c>\n    <g k=\"a\"/>\n  </c>\n  <c i=\"2\"/>\n</r>");
}

// Current: "<a\n</a>" (no '>') and "" for null
TEST(Json2xml, DISABLED_EmptyElements) {
    EXPECT_EQ(convert(R"({"a": {}})").xml, "<a/>");
    EXPECT_EQ(convert(R"({"a": null})").xml, "<a/>");
}

// Current: "<a i=\"1\"/><a i=\"2\"/>" (no new line between root elements)
TEST(Json2xml, DISABLED_SeveralRootElements) {
    EXPECT_EQ(convert(R"({"a": [{"@i": "1"}, {"@i": "2"}]})").xml, "<a i=\"1\"/>\n<a i=\"2\"/>");
    EXPECT_EQ(convert(R"({"a": {"@x": "1"}, "b": {"@y": "2"}})").xml, "<a x=\"1\"/>\n<b y=\"2\"/>");
}

// Current: C++ escaping (std::quoted): '"' and '\' get a backslash, '&', '<' and '>' are kept
TEST(Json2xml, DISABLED_AttributeValuesAreEscaped) {
    EXPECT_EQ(convert(R"({"a": {"@v": "<&>\"'\\"}})").xml, "<a v=\"&lt;&amp;&gt;&quot;'\\\"/>");
}

// Current: success with an empty or invalid xml
TEST(Json2xml, DISABLED_UnsupportedContentFails) {
    EXPECT_FALSE(convert(R"({"a": "text"})").ok);
    EXPECT_FALSE(convert(R"({"a": {"tags": ["x"]}})").ok);
    EXPECT_FALSE(convert(R"({"1a": {}})").ok);
    EXPECT_FALSE(convert(R"({"a": {"@": "1"}})").ok);
}
