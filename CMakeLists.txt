cmake_minimum_required(VERSION 3.12)

project("Plasma-Dissociation" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# External Libraries
add_subdirectory("libs/tomlplusplus")
add_subdirectory("libs/fmt-11.0.2")
add_subdirectory("libs/argparse")

# Program Executables
add_executable(pd-setup-run "src/run.cpp")
target_link_libraries(pd-setup-run PRIVATE shared argparse io-management project-management)

add_executable(plasma-dissociation "src/main.cpp")
target_link_libraries(plasma-dissociation PRIVATE shared argparse io-management project-management)

# QChem Management Library
set(QCHEM_CONFIG_FILES "src/QChemManagement/QChemConfig.cpp" "src/QChemManagement/QChemConfig.hpp")

add_library(qchem-management ${QCHEM_CONFIG_FILES} "src/QChemManagement/QChemManagement.cpp" "src/QChemManagement/QChemManagement.hpp")
target_link_libraries(qchem-management PRIVATE shared io-management)

# Shared Project Features
# TODO: Better Description
add_library(shared INTERFACE)

target_link_libraries(shared INTERFACE fmt::fmt "stdc++fs")
target_compile_definitions(shared INTERFACE MAX_CORES=8)

# Add CMake Sub-Directories
add_subdirectory("src/IOManagement")
add_subdirectory("src/ProjectManagement")