# C++ json2xml converter

C++ header-only converter from Json to XML.

Include [json2xml.hpp](include/ert/json2xml.hpp) into your project.
See more at Integration.

CICD tested with [Codefresh](https://codefresh.io/).

## Integration

[`json2xml.hpp`](https://github.com/testillano/json2xml/blob/master/include/ert/json2xml.hpp) is the single required file in `include/ert` or [released here](https://github.com/testillano/json2xml/releases). You need to add

```cpp
#include <ert/json2xml.hpp>
```

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
$ cmake .
$ make
```

### Component test

On ubuntu you will need python3, pip3 and pytest & xmltodict modules:

```bash
$ apt-get update && apt-get install -y python3 python3-pip
$ pip3 install pytest xmltodict
```

Then, you shall change directory to `ct` and just execute `pytest`:

```bash
$ cd ct
$ pytest
```

### Execute example

```bash
$ examples/json2xml
$ Usage: json2xml <json file|test: harcoded test>
```

#### Passing file

```bash
$ examples/json2xml test.json
```

This will output the xml representation, or an error.

#### Passing 'test' to execute harcoded case

```bash
$ examples/json2xml test
```

Same as above, but using a harcoded json string (check [here](https://github.com/testillano/json2xml/blob/d2778a1891244603284796df6892733b5362324e/examples/main.cpp#L28)).

#### Double check (manual testing)

You could do the opposite (xml to json) using python `xmltodict`:

```bash
$ python3 check.py
```

### Build and test with docker

#### Build

You may remove `cmake` cache from native workflow described above:

```bash
$ find . -name CMakeCache.txt -exec rm {} \;
```

Or perhaps you could execute:

```bash
$ git clean -xdf
```

Now, you will build the compilation docker image, and then build:

```bash
$ docker build -f dev/Dockerfile . -t dev_image:latest
$ docker run --rm -v $PWD:/code -w /code dev_image:latest bash -c "cmake . && make"
```

#### Component test

```bash
$ docker build -f ct/Dockerfile . -t ct_image:latest
$ docker run --rm -v $PWD:/code -w /code/ct ct_image:latest
```

### Restrictions

Arguments in json provided must be prefixed with a special character. You can change it at `JsonSaxConsumer` construction ('@' by default).

### Contributing

Please, execute `astyle` formatting (using [frankwolf image](https://hub.docker.com/r/frankwolf/astyle)) before any pull request:

```bash
$ sources=$(find . -name "*.hpp" -o -name "*.cpp")
$ docker run -it --rm -v $PWD:/data frankwolf/astyle ${sources}
```

