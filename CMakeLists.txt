############
# Versions #
############
cmake_minimum_required (VERSION 3.5.1)
project(ert_json2xml LANGUAGES CXX)

set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MAIN_PROJECT ON)
endif()

###########
# Modules #
###########
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")

###########################
# Options & Configuration #
###########################
option(ERT_JSON2XML_BuildExamples "Build the examples." ${MAIN_PROJECT})
set(ERT_JSON2XML_TARGET_NAME               ${PROJECT_NAME})
set(ERT_JSON2XML_INCLUDE_BUILD_DIR         "${PROJECT_SOURCE_DIR}/include/")

################
# Dependencies #
################
include(FetchContent)

# nlohmann json header-only library (https://github.com/nlohmann/json.git):
# We will point to a lighter repository to speed up the process:
FetchContent_Declare(nlohmann_json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_TAG v3.9.1)
message(STATUS  "Fetching nlohmann_json ... this may take a while")
FetchContent_GetProperties(nlohmann_json)

if(NOT nlohmann_json_POPULATED)
  FetchContent_Populate(nlohmann_json)
  add_subdirectory(${nlohmann_json_SOURCE_DIR} ${nlohmann_json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

##############
# Build type #
##############
include(build_type)
set_cmake_build_type()
message("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

################
# C++ Standard #
################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#########
# Flags #
#########
add_compile_options(
    "-Wall"
    "-Werror"
    $<$<CONFIG:Debug>:--coverage>
)

link_libraries(
    $<$<CONFIG:Debug>:--coverage>
)

############
# Includes #
############
add_library(${ERT_JSON2XML_TARGET_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${ERT_JSON2XML_TARGET_NAME} ALIAS ${ERT_JSON2XML_TARGET_NAME})
target_include_directories(
    ${ERT_JSON2XML_TARGET_NAME}
    INTERFACE
    $<BUILD_INTERFACE:${ERT_JSON2XML_INCLUDE_BUILD_DIR}>
    $<INSTALL_INTERFACE:include>
)

##################
# Subdirectories #
##################
if (ERT_JSON2XML_BuildExamples)
  add_subdirectory( examples )
endif()

