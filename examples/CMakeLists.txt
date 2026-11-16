add_executable (json2xml main.cpp)
target_link_libraries(json2xml PRIVATE ert_json2xml)
target_compile_options(json2xml PRIVATE ${ERT_JSON2XML_COMPILE_OPTIONS})
target_link_options(json2xml PRIVATE ${ERT_JSON2XML_LINK_OPTIONS})
