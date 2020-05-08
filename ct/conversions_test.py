import json
import pytest


def test_001_given_single_nested_test_i_want_to_obtain_corresponding_xml_serialization(json2xml):
    a, b = json2xml("../test.json")
    assert a == b

def test_001_given_two_level_nested_test_i_want_to_obtain_corresponding_xml_serialization(json2xml):
    a, b = json2xml("../test2.json")
    assert a == b



