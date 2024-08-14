cmake_minimum_required(VERSION 3.11)
 
# --- Fetch libs --------------------------------------------------------------
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11)

project(dolores
  LANGUAGES CXX
  VERSION 1.0.0
)

add_executable(dolores src/dolores.cpp)

find_package(std-k REQUIRED)

target_include_directories(dolores
    PRIVATE inc
)

install(
    TARGETS dolores
    DESTINATION /usr/local/bin/
)

target_link_libraries(dolores
    PRIVATE std-k
)

target_compile_features(dolores PUBLIC cxx_std_20)

target_sources(dolores
    PRIVATE src/dolores.cpp
)

# Copyright (c) 2024, Maxamilian Kidd-May
# All rights reserved.

# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

