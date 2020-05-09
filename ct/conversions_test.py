import json
import pytest

def test_001_given_two_level_nested_with_inner_array_with_mixed_nodes__i_want_to_obtain_corresponding_xml_serialization(json2xml):
    a, b = json2xml("../test.json")
    assert a == b

def test_002_given_two_level_nested_with_inner_array_with_single_node__i_want_to_obtain_corresponding_xml_serialization(json2xml):
    a, b = json2xml("../test2.json")
    assert a == b

# TODO: this is failing because we have to fix pop from node stacks after array
@pytest.mark.xfail()
def test_003_given_two_level_nested_with_inner_array_with_two_equal_nodes_and_then_another_first_level_node__i_want_to_obtain_corresponding_xml_serialization(json2xml):
    a, b = json2xml("../test3.json")
    assert a == b
