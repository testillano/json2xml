import json
import logging
import os
import subprocess

import pytest
import xmltodict

#############
# CONSTANTS #
#############
REPO_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXAMPLES_DIR = os.path.join(REPO_DIR, "examples")
# Converter under test: the example program (in-source build by default)
CONVERTER = os.environ.get("JSON2XML_BIN", os.path.join(EXAMPLES_DIR, "json2xml"))

logger = logging.getLogger("CT")

######################
# CLASSES & FIXTURES #
######################

@pytest.fixture(scope="session")
def converter():
  if not os.access(CONVERTER, os.X_OK):
    pytest.fail(f"Converter '{CONVERTER}' not found: build the project first or set JSON2XML_BIN")
  return CONVERTER

@pytest.fixture()
def json2xml(converter):
  """
  Runs the converter on a json file: returns the completed process
  """
  def run(jsonFile):
    return subprocess.run([converter, jsonFile], capture_output=True, text=True)

  yield run

@pytest.fixture()
def roundtrip(json2xml):
  """
  Converts a json file and parses the xml back with xmltodict: returns both dictionaries
  """
  def get_dicts(jsonFile):
    with open(jsonFile) as f:
      jsonDict = json.load(f)
    result = json2xml(jsonFile)
    assert result.returncode == 0, f"conversion failed: {result.stderr}"
    xmlDict = xmltodict.parse(result.stdout)
    logger.info(f"Json read: {jsonDict} | Converted xml: {xmlDict}")
    return jsonDict, xmlDict

  yield get_dicts
