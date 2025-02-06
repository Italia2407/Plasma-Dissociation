# 
set(TOML_READER "TOMLReader.cpp" "TOMLReader.hpp")
set(JSON_READER ) # TODO: Create JSON

add_library(io-management ${TOML_READER} ${JSON_READER})
target_link_libraries(io-management PRIVATE shared)
target_link_libraries(io-management PUBLIC tomlplusplus::tomlplusplus)