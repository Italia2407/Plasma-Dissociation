#include "TOMLParser.hpp"

#include <iostream>

#include <fmt/format.h>

/// @brief 
/// @param a_parsedFile 
TOMLParser::TOMLParser(toml::v3::ex::parse_result a_parsedFile) :
    _parsedFile(a_parsedFile) {}

/// @brief 
/// @param a_filePath The TOML File to be Parsed
/// @return 
std::optional<TOMLParser> TOMLParser::ParseTOMLFile(std::string a_filePath)
{
    toml::v3::ex::parse_result parsedFile;
    try {
        parsedFile = toml::parse_file(a_filePath); 
    }
    // 
    catch (const toml::parse_error& e) {
        std::cerr << fmt::format("ERROR: Failed to Parse {0}", a_filePath) << std::endl;
        std::cerr << '\t' << e.what() << std::endl;

        return {};
    }

    // Create TOMLParser Object
    return TOMLParser(parsedFile);
}

/// @brief Retrieve String Parameter from Parsed File
/// @param a_paramName Name of the Parameter to Retrieve
/// @param a_defaultValue 
/// @return Value of the Parameter, if Existant and of Correct Type
template<> std::optional<std::string> TOMLParser::GetParam(std::string a_paramName, std::optional<std::string> a_defaultValue)
{
    // Return Found String Parameter
    auto parameter = _parsedFile.at_path(a_paramName);
    if (parameter.is_string())
        return parameter.value_or("");

    // Return Default Value
    return a_defaultValue;
}

/// @brief Retrieve Integer Parameter from Parsed File
/// @param a_paramName Name of the Parameter to Retrieve
/// @param a_defaultValue 
/// @return Value of the Parameter, if Existant and of Correct Type
template<> std::optional<int> TOMLParser::GetParam(std::string a_paramName, std::optional<int> a_defaultValue)
{
    // Return Found Integer Parameter
    auto parameter = _parsedFile.at_path(a_paramName);
    if (parameter.is_integer())
        return parameter.value_or(0);

    // Return Default Value
    return a_defaultValue;
}

/// @brief Retrieve Number Parameter from Parsed File
/// @param a_paramName Name of the Parameter to Retrieve
/// @param a_defaultValue 
/// @return Value of the Parameter, if Existant and of Correct Type
template<> std::optional<double> TOMLParser::GetParam(std::string a_paramName, std::optional<double> a_defaultValue)
{
    // Return Found Number Parameter
    auto parameter = _parsedFile.at_path(a_paramName);
    if (parameter.is_number())
        return parameter.value_or(0.0);

    // Return Default Value
    return a_defaultValue;
}

/// @brief Retrieve Boolean Parameter from Parsed File
/// @param a_paramName Name of the Parameter to Retrieve
/// @param a_defaultValue 
/// @return Value of the Parameter, if Existant and of Correct Type
template<> std::optional<bool> TOMLParser::GetParam(std::string a_paramName, std::optional<bool> a_defaultValue)
{
    // Return Found String Parameter
    auto parameter = _parsedFile.at_path(a_paramName);
    if (parameter.is_boolean())
        return parameter.value_or(false);

    // Return Default Value
    return a_defaultValue;
}
