cmake_minimum_required(VERSION 3.14)

project(celebi-project VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(GNUInstallDirs)

add_subdirectory(celebi)
add_subdirectory(celebi-tests)
add_subdirectory(celebi-cli)
