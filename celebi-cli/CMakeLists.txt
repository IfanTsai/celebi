cmake_minimum_required(VERSION 3.14)

project(celebi-cli)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${celebi_SOURCE_DIR} ${celebi_SOURCE_DIR}/include)

set(HEADERS
    cxxopts.hpp
)

add_executable(${PROJECT_NAME}
    ${HEADERS}
    main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE celebi)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
