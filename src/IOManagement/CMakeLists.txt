# 
set(TOML_READER_FILES "TOMLReader.cpp" "TOMLReader.hpp")
set(JSON_READER_FILES ) # TODO: Create JSON

add_library(io-management ${TOML_READER_FILES} ${JSON_READER_FILES})
target_link_libraries(io-management PRIVATE shared)
target_link_libraries(io-management PUBLIC tomlplusplus::tomlplusplus)