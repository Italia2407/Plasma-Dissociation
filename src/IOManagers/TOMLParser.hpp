#pragma once

#include <string>
#include <optional>
#include <sstream>

#include <toml++/toml.hpp>

class TOMLParser
{
private:
    // Private Constructors
    TOMLParser(toml::v3::ex::parse_result a_parsedFile);

    toml::v3::ex::parse_result _parsedFile;
    
public:
    // Builder Method
    static std::optional<TOMLParser> ParseTOMLFile(std::string a_filePath);

    template<typename T>
    std::optional<T> GetParam(std::string a_paramName, std::optional<T> a_defaultValue = {});
    //template<typename T>
    //std::optional<T> GetParam(std::string a_paramName, std::stringstream& r_validationErrorMessage, std::optional<T> a_defaultValue = {});
};