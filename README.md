# C++ json2xml converter

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Documentation](https://codedocs.xyz/testillano/json2xml.svg)](https://codedocs.xyz/testillano/json2xml/index.html)
[![Ask Me Anything !](https://img.shields.io/badge/Ask%20me-anything-1abc9c.svg)](https://github.com/testillano)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/testillano/json2xml/graphs/commit-activity)

C++ header-only converter from Json to XML, based in also header-only [nlohmann json](https://github.com/nlohmann/json) library.



## Restrictions

* All future xml attributes, <u>**are expected to be prefixed**</u>. By default '@' is used (you can change it at `JsonSaxConsumer` constructor), like python [xmltodict](https://github.com/martinblech/xmltodict) project does.

* **XML text** is not supported, so for example key nodes "#text" (default used by xmltodict) is not managed and hence fails as not prefixed like commented above.

* **XML namespaces** are also not expected, and user must expand equivalent top nodes to represent the scope desired.



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

### CMake

#### Embedded

To embed the library directly into an existing CMake project, place the entire source tree in a subdirectory and call `add_subdirectory()` in your `CMakeLists.txt` file:

```cmake
# Typically you don't care so much for a third party library's examples to be
# run from your own project's code.
set(ERT_JSON2XML_BuildExamples OFF CACHE INTERNAL "")

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

FetchContent_Declare(ert_json2xml
  GIT_REPOSITORY https://github.com/testillano/json2xml.git
  GIT_TAG v1.0.1)

FetchContent_GetProperties(ert_json2xml)
if(NOT ert_json_POPULATED)
  FetchContent_Populate(ert_json2xml)
  add_subdirectory(${ert_json2xml_SOURCE_DIR} ${ert_json2xml_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

target_link_libraries(foo PRIVATE ert_json2xml::ert_json2xml)
```

## Build example and test natively

### Build

```bash
$> cmake .
$> make
```

### Component test

CICD is tested with [Codefresh](https://codefresh.io/).
For local component test, on ubuntu you will need python3, pip3 and pytest & xmltodict modules:

```bash
$> apt-get update && apt-get install -y python3 python3-pip
$> pip3 install pytest xmltodict
```

Then, you shall change directory to `ct` and just execute `pytest`:

```bash
$> cd ct
$> pytest
```

Don't forget to build the project first !

### Execute example

A converter example based in the library header is located at `example` directory:

```bash
$> examples/json2xml
$> Usage: json2xml <json file>
```

There are also some test files (`test.<n>.json`) to play with:

#### Passing file

```bash
$> examples/json2xml examples/test.json | tee -a text.xml
```

This will output the xml representation, or an error.

#### Reverse check

You could do the opposite (xml to json) using python `xmltodict` module:

```bash
$> python3 util/xml2json.py test.xml
```

You can also use this beautify perl script for json output, courtesy of [yanOnGithub](https://github.com/jqlang/jq/issues/643#issuecomment-392384015):

```bash
$> python3.9 util/xml2json.py test.xml | perl util/beautifyJson.perl
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
$> docker run --rm -v $PWD:/code -w /code dev_image:latest bash -c "cmake . && make"
```

#### Component test

```bash
$> docker build -f ct/Dockerfile . -t ct_image:latest
$> docker run --rm -v $PWD:/code -w /code/ct ct_image:latest
```

### Contributing

Please, execute `astyle` formatting (using [frankwolf image](https://hub.docker.com/r/frankwolf/astyle)) before any pull request:

```bash
$> sources=$(find . -name "*.hpp" -o -name "*.cpp")
$> docker run -it --rm -v $PWD:/data frankwolf/astyle ${sources}
```

