cmake_minimum_required(VERSION 3.12)

project("Plasma-Dissociation")
set(CMAKE_CXX_STANDARD 17)

set(SOURCES )
set(HEADERS )

add_executable(Plasma-Dissociation "src/main.cpp" ${SOURCES} ${HEADERS})
target_include_directories(Plasma-Dissociation PUBLIC libs/nlohmann-json)