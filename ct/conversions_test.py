import json
import os

import pytest

from conftest import EXAMPLES_DIR

@pytest.mark.parametrize("name", ["test.json", "test2.json", "test3.json"])
def test_001_given_example_file__i_want_to_obtain_equivalent_xml(roundtrip, name):
  a, b = roundtrip(os.path.join(EXAMPLES_DIR, name))
  assert a == b

# xmltodict reads attribute values as strings and empty elements as None, so
# only those json values are compared
@pytest.mark.parametrize("document", [
  {"a": {"@x": "1", "b": {"@y": "2"}}},                          # attributes and children
  {"a": {"b": None, "@x": "1"}},                                 # attribute after a child
  {"a": {"@v": "<&>\"'\\ \t\n\r"}},                              # escaping
  {"a": {"@v": "a\u00f1o \u20ac"}},                              # utf-8
  {"r": {"c": [{"@i": "1"}, {"@i": "2"}], "d": {"@j": "3"}}},    # array followed by a sibling
  {"r": {"c": [{"g": [{"@k": "a"}, {"@k": "b"}]}, {"@i": "2"}]}},# nested repeated elements
  {"a": None},                                                   # empty element
])
def test_002_given_json_document__i_want_to_obtain_equivalent_xml(roundtrip, tmp_path, document):
  path = tmp_path / "document.json"
  path.write_text(json.dumps(document))
  a, b = roundtrip(str(path))
  assert a == b

@pytest.mark.parametrize("content,reason", [
  ('{"a": "text"}', "text content is not supported"),
  ('{"a": {"tags": ["x"]}}', "text content is not supported"),
  ('{"a": [[{}]]}', "nested array"),
  ('{"@a": "1"}', "at top level"),
  ('{"a": {"@x": null}}', "null value for attribute"),
  ('{"1a": {}}', "not a valid XML name"),
  ('{"a": ', "parse error"),
])
def test_003_given_unsupported_json__i_want_to_obtain_an_error(json2xml, tmp_path, content, reason):
  path = tmp_path / "document.json"
  path.write_text(content)
  result = json2xml(str(path))
  assert result.returncode == 1
  assert result.stdout == ""
  assert reason in result.stderr

def test_004_given_missing_file__i_want_to_obtain_an_error(json2xml, tmp_path):
  result = json2xml(str(tmp_path / "missing.json"))
  assert result.returncode == 1
  assert "Cannot read file" in result.stderr
