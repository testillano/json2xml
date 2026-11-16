# googletest (https://github.com/google/googletest)
FetchContent_Declare(googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
  URL_HASH SHA256=8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(unit-test json2xml_test.cpp)
target_link_libraries(unit-test PRIVATE ert_json2xml gtest_main)
target_compile_options(unit-test PRIVATE ${ERT_JSON2XML_COMPILE_OPTIONS})
target_link_options(unit-test PRIVATE ${ERT_JSON2XML_LINK_OPTIONS})

include(GoogleTest)
gtest_discover_tests(unit-test)
