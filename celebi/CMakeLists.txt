cmake_minimum_required(VERSION 3.14)

project(celebi)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${celebi-project_SOURCE_DIR}/highwayhash
)

file(GLOB SRCS ./src/*.cpp)
file(GLOB HEADERS ./include/*.*)

add_library(${PROJECT_NAME} STATIC
  ${HEADERS}
  ${SRCS}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_definitions(${PROJECT_NAME} PRIVATE CELEBI_LIBRARY)
