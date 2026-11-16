// Unit tests of the json to xml SAX consumer (include/ert/json2xml.hpp)

#include <gtest/gtest.h>

#include <ert/json2xml.hpp>

#include <string>

namespace {

struct Result {
    bool ok;
    std::string xml;
    std::string error;
};

Result convert(const std::string& json, int tabSpaces = 2, char attrPrefix = '@') {
    ert::JsonSaxConsumer consumer(tabSpaces, attrPrefix);
    const bool ok = nlohmann::json::sax_parse(json, &consumer);
    return {ok, consumer.getXmlString(), consumer.getError()};
}

// The conversion must fail with no xml and an error containing 'reason'
void expectFailure(const std::string& json, const std::string& reason) {
    const auto r = convert(json);
    EXPECT_FALSE(r.ok) << json;
    EXPECT_TRUE(r.xml.empty()) << json << " -> " << r.xml;
    EXPECT_NE(r.error.find(reason), std::string::npos) << json << " -> '" << r.error << "' lacks '" << reason << "'";
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
    ASSERT_TRUE(r.ok) << r.error;
    EXPECT_EQ(r.xml,
              "<grandfather>\n"
              "  <father>\n"
              "    <child name=\"Jose\" bestFriend=\"Maria\"/>\n"
              "    <child name=\"Maria\" bestFriend=\"Jose\"/>\n"
              "    <student favoriteBook=\"The Gods Themselves\" age=\"21\"/>\n"
              "  </father>\n"
              "</grandfather>");
    EXPECT_TRUE(r.error.empty());
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

TEST(Json2xml, ParentAttributesAndArray) {
    EXPECT_EQ(convert(R"({"node": {"@a": "1", "child": [{"@c": "1"}, {"@c": "2"}]}})").xml,
              "<node a=\"1\">\n  <child c=\"1\"/>\n  <child c=\"2\"/>\n</node>");
}

TEST(Json2xml, AttributesAndChildren) {
    // the parent attributes stay in the parent (they went to the first child before 1.1.0)
    EXPECT_EQ(convert(R"({"a": {"@x": "1", "b": {"@y": "2"}}})").xml, "<a x=\"1\">\n  <b y=\"2\"/>\n</a>");
}

TEST(Json2xml, AttributeAfterChildIsWrittenFirst) {
    EXPECT_EQ(convert(R"({"a": {"b": {}, "@x": "1", "c": null}})").xml, "<a x=\"1\">\n  <b/>\n  <c/>\n</a>");
}

TEST(Json2xml, EmptyElements) {
    EXPECT_EQ(convert(R"({"a": {}})").xml, "<a/>");
    EXPECT_EQ(convert(R"({"a": null})").xml, "<a/>");
    EXPECT_EQ(convert(R"({"a": {"b": {}, "c": null}})").xml, "<a>\n  <b/>\n  <c/>\n</a>");
}

TEST(Json2xml, ArrayFollowedBySibling) {
    EXPECT_EQ(convert(R"({"r": {"c": [{"@i": "1"}, {"@i": "2"}], "d": {"@j": "3"}}})").xml,
              "<r>\n  <c i=\"1\"/>\n  <c i=\"2\"/>\n  <d j=\"3\"/>\n</r>");
}

TEST(Json2xml, ArrayItemAfterNestedArray) {
    // examples/test3.json case: the item keeps the name of its array (before 1.1.0
    // it took the name of the nested array)
    EXPECT_EQ(convert(R"({"r": {"c": [{"g": [{"@k": "a"}]}, {"@i": "2"}]}})").xml,
              "<r>\n  <c>\n    <g k=\"a\"/>\n  </c>\n  <c i=\"2\"/>\n</r>");
}

TEST(Json2xml, ArrayItemsWithChildrenAndNulls) {
    EXPECT_EQ(convert(R"({"r": {"c": [{"@i": "1", "g": [{"@k": "a"}, {"@k": "b"}]}, null, {}]}})").xml,
              "<r>\n"
              "  <c i=\"1\">\n"
              "    <g k=\"a\"/>\n"
              "    <g k=\"b\"/>\n"
              "  </c>\n"
              "  <c/>\n"
              "  <c/>\n"
              "</r>");
}

TEST(Json2xml, EmptyArrayGivesNoElement) {
    EXPECT_EQ(convert(R"({"r": {"c": [], "d": null}})").xml, "<r>\n  <d/>\n</r>");
    EXPECT_EQ(convert(R"({"r": {"c": []}})").xml, "<r/>");
}

TEST(Json2xml, SeveralRootElements) {
    EXPECT_EQ(convert(R"({"a": [{"@i": "1"}, {"@i": "2"}]})").xml, "<a i=\"1\"/>\n<a i=\"2\"/>");
    EXPECT_EQ(convert(R"({"a": {"@x": "1"}, "b": {"@y": "2"}})").xml, "<a x=\"1\"/>\n<b y=\"2\"/>");
    EXPECT_EQ(convert(R"({"a": {}, "b": null})").xml, "<a/>\n<b/>");
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

TEST(Json2xml, AttributeValuesAreEscaped) {
    EXPECT_EQ(convert(R"({"a": {"@v": "<&>\"'\\"}})").xml, "<a v=\"&lt;&amp;&gt;&quot;'\\\"/>");
    EXPECT_EQ(convert(R"({"a": {"@v": "1\t2\n3\r"}})").xml, "<a v=\"1&#9;2&#10;3&#13;\"/>");
}

TEST(Json2xml, Utf8NamesAndValues) {
    // json escapes decoded into UTF-8: "a\u00f1o" (n tilde), "\u20ac" (euro sign), "\u00fc" (u diaeresis)
    EXPECT_EQ(convert(R"({"a\u00f1o": {"@price": "10 \u20ac", "@\u00f1": "\u00fc"}})").xml,
              "<a\xc3\xb1o price=\"10 \xe2\x82\xac\" \xc3\xb1=\"\xc3\xbc\"/>");
}

TEST(Json2xml, ValidNameCharacters) {
    EXPECT_EQ(convert(R"({"_a-1.b:c": {"@x_2-y.z": "v"}})").xml, "<_a-1.b:c x_2-y.z=\"v\"/>");
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
    EXPECT_EQ(convert(R"({"a": {"-x": "1", "b": null}})", 2, '-').xml, "<a x=\"1\">\n  <b/>\n</a>");
    // with another prefix, '@' is not an attribute (and not a valid element name either)
    const auto r = convert(R"({"a": {"@x": "1"}})", 2, '-');
    EXPECT_FALSE(r.ok);
    EXPECT_NE(r.error.find("not a valid XML name"), std::string::npos) << r.error;
}

TEST(Json2xml, XmlStreamMatchesString) {
    ert::JsonSaxConsumer consumer(2);
    ASSERT_TRUE(nlohmann::json::sax_parse(R"({"a": {"@x": "1"}})", &consumer));
    EXPECT_EQ(consumer.getXmlStream().str(), consumer.getXmlString());
}

TEST(Json2xml, ConsumerReuse) {
    ert::JsonSaxConsumer consumer(2);
    ASSERT_TRUE(nlohmann::json::sax_parse(R"({"a": {"@x": "1"}})", &consumer));
    ASSERT_TRUE(nlohmann::json::sax_parse(R"({"b": null})", &consumer));
    EXPECT_EQ(consumer.getXmlString(), "<b/>");

    // after a failure
    EXPECT_FALSE(nlohmann::json::sax_parse(R"({"a": "text"})", &consumer));
    EXPECT_TRUE(consumer.getXmlString().empty());
    EXPECT_FALSE(consumer.getError().empty());
    ASSERT_TRUE(nlohmann::json::sax_parse(R"({"c": {}})", &consumer));
    EXPECT_EQ(consumer.getXmlString(), "<c/>");
    EXPECT_TRUE(consumer.getError().empty());
}

//////////////
// Failures //
//////////////

TEST(Json2xml, TextContentIsNotSupported) {
    expectFailure(R"({"a": "text"})", "string value at '/a': text content is not supported");
    expectFailure(R"({"a": {"b": 1}})", "number value at '/a/b': text content");
    expectFailure(R"({"a": {"b": true}})", "boolean value at '/a/b': text content");
    expectFailure(R"({"a": {"tags": ["x", "y"]}})", "string value at '/a/tags[]': text content");
    expectFailure(R"({"a": "x"})", "text content");
}

TEST(Json2xml, NestedArraysAreNotSupported) {
    expectFailure(R"({"a": [[{}]]})", "nested array at '/a[]'");
}

TEST(Json2xml, TopLevelMustBeAnObjectWithElements) {
    expectFailure("[]", "a json object is expected at top level");
    expectFailure("[{\"a\": {}}]", "a json object is expected at top level");
    expectFailure("\"x\"", "a json object is expected at top level");
    expectFailure("1", "a json object is expected at top level");
    expectFailure("null", "a json object is expected at top level");
    expectFailure("{}", "the json object has no element");
    expectFailure(R"({"@a": "1"})", "attribute '@a' at top level");
}

TEST(Json2xml, InvalidAttributeValues) {
    expectFailure(R"({"a": {"@x": {}}})", "object value for attribute '/a/@x'");
    expectFailure(R"({"a": {"@x": ["1"]}})", "array value for attribute '/a/@x'");
    expectFailure(R"({"a": {"@x": null}})", "null value for attribute '/a/@x'");
    expectFailure(R"({"a": {"@x": "1", "@x": "2"}})", "repeated attribute '/a/@x'");
    expectFailure(R"({"a": {"@x": "\u0001"}})", "character not allowed in XML");
}

TEST(Json2xml, InvalidNames) {
    expectFailure(R"({"": {}})", "element name '' at '/' is not a valid XML name");
    expectFailure(R"({"a": {"@": "1"}})", "attribute name '@' at '/a/@' is not a valid XML name");
    expectFailure(R"({"1a": {}})", "element name '1a'");
    expectFailure(R"({"a b": {}})", "element name 'a b'");
    expectFailure(R"({"a": {"#text": "x"}})", "element name '#text'");
    expectFailure(R"({"a": {"@x y": "1"}})", "attribute name '@x y'");
    expectFailure(R"({"a<": {}})", "element name 'a<'");
}

TEST(Json2xml, UnsupportedContentFails) {
    // failed with success and an empty or invalid xml before 1.1.0 (details in the tests above)
    expectFailure(R"({"a": "text"})", "text content");
    expectFailure(R"({"a": {"tags": ["x"]}})", "text content");
    expectFailure(R"({"1a": {}})", "not a valid XML name");
    expectFailure(R"({"a": {"@": "1"}})", "not a valid XML name");
}

TEST(Json2xml, JsonParseErrorFails) {
    expectFailure(R"({"a": )", "parse error");
    expectFailure("", "parse error");
    expectFailure(R"({"a": {}} trailing)", "parse error");
}
