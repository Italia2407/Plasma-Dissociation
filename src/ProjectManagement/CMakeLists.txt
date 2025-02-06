#
set(MOLECULE_PROJECT "MoleculeProject.cpp" "MoleculeProject.hpp")

add_library(project-management ${MOLECULE_PROJECT})
target_link_libraries(project-management PRIVATE shared io-management)
target_compile_definitions(project-management INTERFACE TRAJECTORIES_FOLDER_NAME="trajectories" GEOMETRIES_FOLDER_NAME="geometries")