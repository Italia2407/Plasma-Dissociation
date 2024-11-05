cmake_minimum_required(VERSION 3.12)

project("Plasma-Dissociation")
set(CMAKE_CXX_STANDARD 17)

set(SOURCES )
set(HEADERS )

add_executable(Plasma-Dissociation "src/main.cpp" ${SOURCES} ${HEADERS})
target_include_directories(Plasma-Dissociation PRIVATE "libs/tomlplusplus/include")

#  
find_package(Boost 1.74 REQUIRED COMPONENTS date_time program_options)
target_link_libraries(Plasma-Dissociation PRIVATE Boost::date_time Boost::program_options)