import xmltodict
import json
import sys

if len(sys.argv) != 2:
  print("Usage: python3 xml2json.py <xml file>")
  sys.exit(1)

with open(sys.argv[1]) as fd:
  data_dict = xmltodict.parse(fd.read())

indentation=4
json_data=json.dumps(data_dict, indent=indentation) # add 'sort_keys=True)' if desired
print(json_data)
