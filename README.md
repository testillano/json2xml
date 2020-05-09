# json2xml

C++ Converter from Json to XML

## Build

    cmake .
    make

## Execute

    ./json2xml

This will show the correct usage:

### Passing file

    ./json2xml test.json

This will output the xml representation, or an error.

### Passing 'test'

    ./json2xml test

Same as above, but using a harcoded json string (check [here](https://github.com/testillano/json2xml/blob/master/json2xml.cpp#L141)).

## Restrictions

Arguments in json provided must be prefixed with a special character. You can change it at `JsonSaxConsumer` construction ('@' by default).

## Double check

You could do the opposite (xml to json) using python `xmltodict`:

    pip3 install xmltodict
    python3 check.py

## Component test

Go to `./ct` and execute `pytest`.
