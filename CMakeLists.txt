cmake_minimum_required(VERSION 3.12)

project("Plasma-Dissociation" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

#  Retrieve Boost Libraries
include_directories(${Boost_INCLUDE_DIRS})
find_package(Boost 1.67 REQUIRED COMPONENTS date_time program_options)

# 
add_subdirectory("libs/fmt-11.0.2")

# 
set(ASSETS_PATH "${CMAKE_SOURCE_DIR}/assets")
set(RESULTS_PATH "${CMAKE_SOURCE_DIR}/out/results")

# Source Code 

set(TOML_PARSER_FILES "src/IOManagers/TOMLParser.hpp" "src/IOManagers/TOMLParser.cpp")
set(MOLECULE_TEST_FILES "src/IOManagers/MoleculeTest.hpp" "src/IOManagers/MoleculeTest.cpp" ${TOML_PARSER_FILES})
set(QCHEM_CONFIG_FILES "src/IOManagers/QChemConfig.hpp" "src/IOManagers/QChemConfig.cpp" ${TOML_PARSER_FILES})

# Primary Run Executable
add_executable(Plasma-Dissociation "src/PlasmaDissociation.cpp" ${MOLECULE_TEST_FILES})
target_compile_definitions(Plasma-Dissociation PRIVATE ASSETS_PATH="${ASSETS_PATH}" RESULTS_PATH="${RESULTS_PATH}" MAX_CORES=8)

target_include_directories(Plasma-Dissociation PRIVATE "libs/tomlplusplus/include")
target_link_libraries(Plasma-Dissociation PRIVATE Boost::date_time Boost::program_options fmt::fmt)

# Geometry Creation Executable
add_executable(Create-Geometry "src/CreateGeometry.cpp" ${MOLECULE_TEST_FILES})
target_compile_definitions(Create-Geometry PRIVATE ASSETS_PATH="${ASSETS_PATH}" RESULTS_PATH="${RESULTS_PATH}" MAX_CORES=8)

target_include_directories(Create-Geometry PRIVATE "libs/tomlplusplus/include")
target_link_libraries(Create-Geometry PRIVATE Boost::date_time Boost::program_options fmt::fmt)

# QChem Executable
add_executable(Run-QChem "src/RunQChem.cpp" ${QCHEM_CONFIG_FILES})
target_compile_definitions(Run-QChem PRIVATE ASSETS_PATH="${ASSETS_PATH}" RESULTS_PATH="${RESULTS_PATH}" MAX_CORES=8)

target_include_directories(Run-QChem PRIVATE "libs/tomlplusplus/include")
target_link_libraries(Run-QChem PRIVATE Boost::date_time Boost::program_options fmt::fmt)