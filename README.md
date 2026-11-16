# C++ json2xml converter

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Pages](https://img.shields.io/github/deployments/testillano/json2xml/github-pages?label=pages)](https://testillano.github.io/json2xml/)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](https://testillano.github.io/json2xml/doxygen/)
[![Coverage Status](https://img.shields.io/endpoint?url=https://testillano.github.io/json2xml/coverage/badge.json)](https://testillano.github.io/json2xml/coverage)
[![Ask Me Anything !](https://img.shields.io/badge/Ask%20me-anything-1abc9c.svg)](https://github.com/testillano)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/testillano/json2xml/graphs/commit-activity)
[![CI](https://github.com/testillano/json2xml/actions/workflows/ci.yml/badge.svg)](https://github.com/testillano/json2xml/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/testillano/json2xml)](https://github.com/testillano/json2xml/releases)

C++ header-only converter from Json to XML, based in also header-only [nlohmann json](https://github.com/nlohmann/json) library.



## Restrictions

The json layout is the one of python [xmltodict](https://github.com/martinblech/xmltodict), restricted to elements and attributes:

* The top level json value must be an object: each key is a root element (normally one; several give an XML fragment).
* A key with an object value is an element. Its keys **prefixed** with `@` (you can change it at `JsonSaxConsumer` constructor) are the attributes, with string, number or boolean values (escaped as needed); the other keys are the child elements. Attributes are written first, wherever they are in the object.
* A key with an array value is a repeated element: one per item (objects, or `null` for empty elements).
* A key with a `null` value, or an empty object, is an empty element (`<a/>`).

Not supported (the conversion fails):

* **XML text**: an element key with a scalar value (`{"a": "text"}`), an array of scalars, or the `#text` key used by xmltodict.
* **XML namespaces** are also not expected, and user must expand equivalent top nodes to represent the scope desired.
* Nested arrays, attributes at top level, with object, array or `null` values or repeated, invalid XML names and characters not allowed in XML 1.0.

In summary, this converter is intended to be used for simple xml configurations with predictable and controlled content:

```json
{
    "grandfather": {
        "father": {
            "child": [
                {
                    "@name": "Jose",
                    "@bestFriend": "Maria"
                },
                {
                    "@name": "Maria",
                    "@bestFriend": "Jose"
                }
            ],
            "student": {
                "@favoriteBook": "The Gods Themselves",
                "@age": "21"
            }
        }
    }
}
```

turns into:

```xml
<grandfather>
  <father>
    <child name="Jose" bestFriend="Maria"/>
    <child name="Maria" bestFriend="Jose"/>
    <student favoriteBook="The Gods Themselves" age="21"/>
  </father>
</grandfather>
```



## Integration

[`json2xml.hpp`](https://github.com/testillano/json2xml/blob/master/include/ert/json2xml.hpp) is the single required file in `include/ert` or [released here](https://github.com/testillano/json2xml/releases). You need to add

```cpp
#include <ert/json2xml.hpp> // or any other desired location within your project
```

You must also integrate `nlohmann/json` header-only file ([json.hpp](https://raw.githubusercontent.com/nlohmann/json/develop/include/nlohmann/json.hpp)) in your project.

### Usage

```cpp
ert::JsonSaxConsumer consumer(2); // indentation spaces (4 by default), attribute prefix ('@' by default)
if (nlohmann::json::sax_parse(jsonString, &consumer)) {
    std::string xml = consumer.getXmlString();
}
else {
    std::string reason = consumer.getError(); // json parse error, or something not supported (see restrictions)
}
```

On failure, the xml is empty (before 1.1.0 it held the json parse error, and unsupported content gave a wrong xml). A consumer can be reused: every parse starts from scratch.

The full API is documented [here](https://testillano.github.io/json2xml/doxygen/).

### CMake

#### Embedded

To embed the library directly into an existing CMake project, place the entire source tree in a subdirectory and call `add_subdirectory()` in your `CMakeLists.txt` file:

```cmake
# Typically you don't care so much for a third party library's examples to be
# run from your own project's code.
set(ERT_JSON2XML_BuildExamples OFF CACHE INTERNAL "")
set(ERT_JSON2XML_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(ert_json2xml)
...
add_library(foo ...)
...
target_link_libraries(foo PRIVATE ert_json2xml::ert_json2xml)
```

##### Embedded (FetchContent)

Since CMake v3.11,
[FetchContent](https://cmake.org/cmake/help/v3.11/module/FetchContent.html) can be used to automatically download the repository as a dependency at configure type.

Example:
```cmake
include(FetchContent)

set(ERT_JSON2XML_BuildExamples OFF CACHE INTERNAL "")
set(ERT_JSON2XML_BuildTests OFF CACHE INTERNAL "")

FetchContent_Declare(ert_json2xml
  GIT_REPOSITORY https://github.com/testillano/json2xml.git
  GIT_TAG v1.1.0)
FetchContent_MakeAvailable(ert_json2xml)

target_link_libraries(foo PRIVATE ert_json2xml::ert_json2xml)
```

## Build example and test natively

### Build

CMake 3.14 or newer is required (dependencies are fetched at configure time: `nlohmann/json` and, for the unit tests, `googletest`):

```bash
$> cmake .
$> make
```

### Unit test

Built by default (option `ERT_JSON2XML_BuildTests`):

```bash
$> ut/unit-test
- or -
$> ctest --output-on-failure
```

### Coverage

A `Debug` build is instrumented for coverage (`lcov` is needed for the report):

```bash
$> cmake -DCMAKE_BUILD_TYPE=Debug . && make && ut/unit-test
$> lcov --capture --directory . --output-file total.info && lcov --extract total.info "$PWD/include/*" --output-file coverage.info
$> genhtml coverage.info --output-directory coverage
```

The CI ([GitHub Actions](https://github.com/testillano/json2xml/actions/workflows/ci.yml)) publishes the [coverage report](https://testillano.github.io/json2xml/coverage) and the [doxygen documentation](https://testillano.github.io/json2xml/doxygen/) (local generation: `cd docs/doxygen && doxygen`) on [GitHub Pages](https://testillano.github.io/json2xml/).

### Component test

The example converter is checked with `pytest`: the xml is parsed back with python `xmltodict` and compared to the json, and the unsupported cases must fail. On ubuntu you will need pytest & xmltodict modules:

```bash
$> apt-get update && apt-get install -y python3-pytest python3-xmltodict
```

Then, you shall change directory to `ct` and just execute `pytest` (the converter is `examples/json2xml`, or the one given by `JSON2XML_BIN`):

```bash
$> cd ct
$> pytest
```

Don't forget to build the project first !

### Execute example

A converter example based in the library header is located at `examples` directory:

```bash
$> examples/json2xml
$> Usage: json2xml <json file>
```

There are also some test files (`examples/test*.json`) to play with:

#### Passing file

```bash
$> examples/json2xml examples/test.json | tee test.xml
```

This will output the xml representation, or an error (on standard error, with exit code 1).

#### Reverse check

You could do the opposite (xml to json) using python `xmltodict` module:

```bash
$> python3 util/xml2json.py test.xml
```

You can also use this beautify perl script for json output, courtesy of [yanOnGithub](https://github.com/jqlang/jq/issues/643#issuecomment-392384015):

```bash
$> python3 util/xml2json.py test.xml | perl util/beautifyJson.perl
```

### Build and test with docker

#### Build

You may remove `cmake` cache from native workflow described above:

```bash
$> find . -name CMakeCache.txt -exec rm {} \;
```

Or perhaps you could execute:

```bash
$> git clean -xdf
```

Now, you will build the compilation docker image, and then build:

```bash
$> docker build -f dev/Dockerfile . -t dev_image:latest
$> docker run --rm -u $(id -u):$(id -g) -v $PWD:/code -w /code dev_image:latest bash -c "cmake . && make && ut/unit-test"
```

#### Component test

```bash
$> docker build -f ct/Dockerfile . -t ct_image:latest
$> docker run --rm -v $PWD:/code -w /code/ct ct_image:latest
```

### Contributing

Please, execute `astyle` formatting (using [frankwolf image](https://hub.docker.com/r/frankwolf/astyle)) before any pull request:

```bash
$> sources=$(find include examples ut -name "*.hpp" -o -name "*.cpp")
$> docker run -it --rm -v $PWD:/data frankwolf/astyle ${sources}
```

