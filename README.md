# json2xml

C++ Converter from Json to XML

## Build

> cmake .
> make

## Execute

> ./json2xml

This will show the correct usage:

### Passing file

> ./json2xml test.json

This will output the xml representation, or an error.

### Passing 'test'

> ./json2xml test

Same as above, but using a harcoded json string (check json2xml.cpp, line 135).

## Restrictions

Arguments in json provided must be prefixed with '@'.

## Double check

You could do the opositte (xml to json) using python `xmltodict`:

> pip3 install xmltodict

For example, take a look to `check.py` script, which injects the xml got from `json2xml test.json`:

> python3 check.py > result.json
> diff test.json result.json
> echo $?
> 0

