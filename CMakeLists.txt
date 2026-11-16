############
# Versions #
############
cmake_minimum_required (VERSION 3.14)
project(ert_json2xml VERSION 1.1.0 LANGUAGES CXX)

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
option(ERT_JSON2XML_BuildTests "Build the unit tests." ${MAIN_PROJECT})
set(ERT_JSON2XML_TARGET_NAME               ${PROJECT_NAME})
set(ERT_JSON2XML_INCLUDE_BUILD_DIR         "${PROJECT_SOURCE_DIR}/include/")

################
# Dependencies #
################
include(FetchContent)
# Extracted files get the extraction time (CMake >= 3.24 warns when unset)
if (POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

# nlohmann json header-only library (https://github.com/nlohmann/json), unless the
# parent project already provides it. The release tarball is much lighter than
# the repository.
if (NOT TARGET nlohmann_json::nlohmann_json)
  message(STATUS "Fetching nlohmann_json ...")
  FetchContent_Declare(nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d)
  FetchContent_MakeAvailable(nlohmann_json)
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
# Only for the targets of this project (examples, unit tests), not for the
# fetched dependencies nor for a parent project. Debug builds are instrumented
# for coverage.
set(ERT_JSON2XML_COMPILE_OPTIONS "-Wall" "-Werror" $<$<CONFIG:Debug>:--coverage>)
set(ERT_JSON2XML_LINK_OPTIONS $<$<CONFIG:Debug>:--coverage>)

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
target_link_libraries(${ERT_JSON2XML_TARGET_NAME} INTERFACE nlohmann_json::nlohmann_json)

##################
# Subdirectories #
##################
if (ERT_JSON2XML_BuildExamples)
  add_subdirectory( examples )
endif()

if (ERT_JSON2XML_BuildTests)
  enable_testing()
  add_subdirectory( ut )
endif()
