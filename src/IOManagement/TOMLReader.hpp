#pragma once

#include <string>
#include <optional>

#include <toml++/toml.hpp>

namespace IOManagement {

class TOMLReader
{
private:
    TOMLReader(toml::v3::ex::parse_result a_parseResult);
    toml::v3::ex::parse_result _parseResult;

public:
    // Builder Method
    static std::optional<TOMLReader> ParseTOMLFile(std::string a_filePath);

    template<typename T>
    std::optional<T> GetParameter(std::string a_paramName, std::optional<T> a_defaultValue = {});
};

}