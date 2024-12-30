#include "TOMLReader.hpp"

#include <iostream>
#include <fmt/format.h>
namespace IOManagement {

/// @brief 
/// @param a_parsedFile 
TOMLReader::TOMLReader(toml::v3::ex::parse_result a_parseResult) :
    _parseResult(a_parseResult) {}

/// @brief Create TOMLReader Object from TOML File
/// @param a_filePath The TOML File to be Parsed
/// @return The Created TOMLReader Object, if Successful
std::optional<TOMLReader> TOMLReader::ParseTOMLFile(std::string a_filePath)
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

    // Return the Created Object
    return TOMLReader(parsedFile);
}

/// @brief Retrieve String Parameter
/// @param a_paramName Name of the Parameter to Retrieve
/// @param a_defaultValue 
/// @return Value of the Parameter, if Existant and of Correct Type
template<> std::optional<std::string> TOMLReader::GetParameter(std::string a_paramName, std::optional<std::string> a_defaultValue)
{
    // Return Found String Parameter
    auto parameter = _parseResult.at_path(a_paramName);
    if (parameter.is_string())
        return parameter.value_or("");

    // Return Default Value
    return a_defaultValue;
}

/// @brief Retrieve Integer Parameter
/// @param a_paramName Name of the Parameter to Retrieve
/// @param a_defaultValue 
/// @return Value of the Parameter, if Existant and of Correct Type
template<> std::optional<int> TOMLReader::GetParameter(std::string a_paramName, std::optional<int> a_defaultValue)
{
    // Return Found Integer Parameter
    auto parameter = _parseResult.at_path(a_paramName);
    if (parameter.is_integer())
        return parameter.value_or(0);

    // Return Default Value
    return a_defaultValue;
}

/// @brief Retrieve Number Parameter
/// @param a_paramName Name of the Parameter to Retrieve
/// @param a_defaultValue 
/// @return Value of the Parameter, if Existant and of Correct Type
template<> std::optional<double> TOMLReader::GetParameter(std::string a_paramName, std::optional<double> a_defaultValue)
{
    // Return Found Number Parameter
    auto parameter = _parseResult.at_path(a_paramName);
    if (parameter.is_number())
        return parameter.value_or(0.0);

    // Return Default Value
    return a_defaultValue;
}

/// @brief Retrieve Boolean Parameter
/// @param a_paramName Name of the Parameter to Retrieve
/// @param a_defaultValue 
/// @return Value of the Parameter, if Existant and of Correct Type
template<> std::optional<bool> TOMLReader::GetParameter(std::string a_paramName, std::optional<bool> a_defaultValue)
{
    // Return Found String Parameter
    auto parameter = _parseResult.at_path(a_paramName);
    if (parameter.is_boolean())
        return parameter.value_or(false);

    // Return Default Value
    return a_defaultValue;
}

}