cmake_minimum_required(VERSION 3.8)

project("Plasma-Dissociation")
set(CMAKE_CXX_STANDARD 17)

set(SOURCES )
set(HEADERS )

add_executable(Plasma-Dissociation "src/main.cpp" ${SOURCES} ${HEADERS})