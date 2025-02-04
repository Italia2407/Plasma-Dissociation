cmake_minimum_required(VERSION 3.12)

project("Plasma-Dissociation" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# External Libraries
add_subdirectory("libs/tomlplusplus")
add_subdirectory("libs/fmt-11.0.2")
add_subdirectory("libs/argparse")

# Program Executables
set(MOLECULE_TEST_FILES "src/MoleculeTest.cpp" "src/MoleculeTest.hpp")

add_executable(pd-setup-run "src/run.cpp" ${MOLECULE_TEST_FILES})
target_link_libraries(pd-setup-run PRIVATE shared argparse io-management)

add_executable(plasma-dissociation "src/main.cpp" ${MOLECULE_TEST_FILES})
target_link_libraries(plasma-dissociation PRIVATE shared argparse io-management)

# QChem Management Library
set(QCHEM_CONFIG_FILES "src/QChemManagement/QChemConfig.cpp" "src/QChemManagement/QChemConfig.hpp")

add_library(qchem-management ${QCHEM_CONFIG_FILES} "src/QChemManagement/QChemManagement.cpp" "src/QChemManagement/QChemManagement.hpp")
target_link_libraries(qchem-management PRIVATE shared io-management)

# IO Management Library
set(TOML_READER_FILES "src/IOManagement/TOMLReader.cpp" "src/IOManagement/TOMLReader.hpp")
set(JSON_READER_FILES ) # TODO: Create JSON

add_library(io-management ${TOML_READER_FILES} ${JSON_READER_FILES})
target_link_libraries(io-management PRIVATE shared)
target_link_libraries(io-management PUBLIC tomlplusplus::tomlplusplus)

# Shared Project Features
# TODO: Better Description
add_library(shared INTERFACE)

target_link_libraries(shared INTERFACE fmt::fmt)
target_compile_definitions(shared INTERFACE MAX_CORES=8)