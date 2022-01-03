cmake_minimum_required(VERSION 3.14)

project(celebi-tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${celebi_SOURCE_DIR} ${celebi_SOURCE_DIR}/include)

set(HEADERS
    catch.hpp
    tests.h
)

add_executable(${PROJECT_NAME}
    ${HEADERS}
    dbmanagement-tests.cpp
    keyvalue-tests.cpp
    performance-tests.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE celebi)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
