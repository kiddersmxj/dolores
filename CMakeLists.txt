cmake_minimum_required(VERSION 3.11)
set(CMAKE_BUILD_TYPE Debug)

# Project definition
project(dolores
  LANGUAGES CXX
  VERSION 1.0.0
)

include(FetchContent)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch libcurl if not found
find_package(CURL QUIET)
if(NOT CURL_FOUND)
    message(STATUS "libcurl not found, fetching...")
    FetchContent_Declare(
        curl
        URL https://github.com/curl/curl/archive/refs/tags/curl-7_82_0.tar.gz
    )
    FetchContent_MakeAvailable(curl)
endif()

if(NOT TARGET CURL::libcurl)
    add_library(CURL::libcurl INTERFACE IMPORTED)
    target_include_directories(CURL::libcurl INTERFACE ${curl_SOURCE_DIR}/include)
    target_link_libraries(CURL::libcurl INTERFACE ${curl_BINARY_DIR}/lib/libcurl.a)
endif()

# Fetch nlohmann_json (Header-only)
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found, fetching...")
    FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
    )
    FetchContent_MakeAvailable(json)
endif()

include(FetchContent)
 
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG main
)
 
FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# Add the executable
add_executable(dolores
    src/dolores.cpp
)

# Include directories
target_include_directories(dolores
    PRIVATE inc
)

# Link libraries
target_link_libraries(dolores
    PRIVATE std-k
    PRIVATE CURL::libcurl
    PRIVATE nlohmann_json::nlohmann_json
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
    PRIVATE std-k
)

# Install target
install(
    TARGETS dolores
    DESTINATION /usr/local/bin/
)

# Specify the source files for the project
target_sources(dolores
    PRIVATE src/dolores.cpp
    PRIVATE src/curl.cpp
    PRIVATE src/tokeniser.cpp
    PRIVATE src/markdown.cpp
    PRIVATE src/display.cpp
    PRIVATE src/database.cpp
    PRIVATE src/messages.cpp
    PRIVATE src/shorts.cpp
    PRIVATE src/vim.cpp
)

# Copyright (c) 2024, Maxamilian Kidd-May
# All rights reserved.

# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

