# Keep sorted
import json
import logging
import os
import pytest
import subprocess
import xmltodict

#############
# CONSTANTS #
#############

######################
# CLASSES & FIXTURES #
######################

# Logging
class MyLogger:

  # CRITICAL ERROR WARNING INFO DEBUG NOSET
  def setLevelInfo(): logging.getLogger().setLevel(logging.INFO)
  def setLevelDebug(): logging.getLogger().setLevel(logging.DEBUG)

  def error(message): logging.getLogger().error(message)
  def warning(message): logging.getLogger().warning(message)
  def info(message): logging.getLogger().info(message)
  def debug(message): logging.getLogger().debug(message)

@pytest.fixture(scope='session')
def mylogger():
  return MyLogger

MyLogger.logger = logging.getLogger('CT')

@pytest.fixture()
def json2xml():
  """
  Executes the json2xml tool and returns the original json dictionary and the one obtained from xml output tool result
  """
  def get_dicts(testFile):
    # Before conversion:
    jsonDict = None
    try:
      with open(testFile) as jsonFile:
        jsonDict = json.load(jsonFile)
    except OSError:
        assert False, "Could not open/read file '{}'".format(testFile)

    # After conversion:
    converter = os.getcwd() + "/../examples/json2xml"
    args = (converter, testFile)
    popen = subprocess.Popen(args, stdout=subprocess.PIPE)
    popen.wait()
    output = popen.stdout.read()
    xmlDict = xmltodict.parse(output)

    match = (jsonDict == xmlDict)
    log = "Json read: {jsonDict} | Converted xml: {xmlDict}".format(jsonDict=jsonDict, xmlDict=xmlDict)
    if match: MyLogger.info(log)
    else: MyLogger.error(log)

    return jsonDict, xmlDict

  yield get_dicts

