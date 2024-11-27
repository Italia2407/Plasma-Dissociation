cmake_minimum_required(VERSION 3.12)

project("Plasma-Dissociation" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

#  Retrieve Boost Libraries
include_directories(${Boost_INCLUDE_DIRS})
find_package(Boost 1.74 REQUIRED COMPONENTS date_time program_options)

# 
add_subdirectory("libs/fmt-11.0.2")

set(RUN_SOURCES "src/InputParser.cpp")
set(RUN_HEADERS "src/InputParser.hpp")

add_executable(Plasma-Dissociation "src/run/main.cpp" ${RUN_SOURCES} ${RUN_HEADERS})
target_compile_definitions(Plasma-Dissociation PRIVATE ASSETS_PATH="${CMAKE_SOURCE_DIR}/assets" MAX_CORES=8)

target_include_directories(Plasma-Dissociation PRIVATE "libs/tomlplusplus/include")
target_link_libraries(Plasma-Dissociation PRIVATE Boost::date_time Boost::program_options fmt::fmt)