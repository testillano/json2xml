# C++ json2xml converter

C++ header-only converter from Json to XML.
Include [./json2xml.hpp](./json2xml.hpp) into your project.

CICD tested with [Codefresh](https://codefresh.io/).

## Build example and test natively

### Build

    cmake .
    make

### Component test

On ubuntu you will need python3, pip3 and pytest & xmltodict modules:

    apt-get update && apt-get install -y python3 python3-pip
    pip3 install pytest xmltodict

Then, you shall change directory to `ct` and just execute `pytest`:

    cd ct
    pytest

### Execute example

    ./json2xml

This will show the correct usage:

    Usage: json2xml <json file|test: harcoded test>

#### Passing file

    ./json2xml test.json

This will output the xml representation, or an error.

#### Passing 'test' to execute harcoded case

    ./json2xml test

Same as above, but using a harcoded json string (check [here](https://github.com/testillano/json2xml/blob/master/json2xml.cpp#L141)).

#### Double check (manual testing)

You could do the opposite (xml to json) using python `xmltodict`:

    python3 check.py

### Build and test with docker

#### Build

You may remove cmake cache from native workflow described above:

    rm CMakeCache.txt

Now, you will build the compilation docker image, and then build:

    docker build -f dev/Dockerfile . -t dev_image:latest
    docker run --rm -v $PWD:/code -w /code dev_image:latest bash -c "cmake . && make"

#### Component test

    docker build -f ct/Dockerfile . -t ct_image:latest
    docker run --rm -v $PWD:/code -w /code/ct ct_image:latest

### Restrictions

Arguments in json provided must be prefixed with a special character. You can change it at `JsonSaxConsumer` construction ('@' by default).

### Contributing

Please, execute `astyle` formatting before any pull request:

    docker pull frankwolf/astyle
    docker run -it --rm -v $PWD:/data frankwolf/astyle ./json2xml.hpp ./example.cpp

