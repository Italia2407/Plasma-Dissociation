#include "QChemConfig.hpp"

#include "../IOManagement/TOMLReader.hpp"

#include <iostream>
#include <sstream>

#include <type_traits>

#include <fmt/format.h>
#include <toml++/toml.hpp>

namespace QChemManagement {

/// @brief Check whether the Values of the QChem Configuration are Valid
bool QChemConfig::AreValuesValid() const
{
    // TODO: Implement actual Checks
    return true;
}

/// @brief Create QChemConfig Object from TOML File
/// @param a_fileName The TOML File to be Parsed
/// @return The Created QChemConfig Object, if Successful
std::optional<QChemConfig> QChemConfig::CreateFromTOMLFile(std::string a_filePath)
{
    // Ensure TOML File was Parsed Correctly
    auto tomlParser = IOManagement::TOMLReader::ParseTOMLFile(a_filePath);
    if (!tomlParser)
        return {};

    // Create QChemConfig Object, Ensuring Parameters are all Valid
    QChemConfig qchemConfig;
    std::stringstream validationErrorMessage;

    // TODO: Not available prior to C++20
    // TODO: Find Possible place for this Function to share code
    auto setParamValue = [&]<typename T>(T& r_paramReference, std::string a_paramName, std::optional<T> a_defaultValue = {}) {
        auto parameter = tomlParser.value().GetParameter<T>(a_paramName, a_defaultValue);

        // Ensure Parameter was Correctly Found
        if (!parameter) {
            if (!validationErrorMessage.str().empty())
                validationErrorMessage << '\n';

            // Get Expected Type Name
            std::string expectedTypeName = "";
            if (std::is_same<T, std::string>())
                expectedTypeName = "String";
            else if (std::is_same<T, int>())
                expectedTypeName = "Integer";
            else if (std::is_same<T, double>())
                expectedTypeName = "Number";
            else if (std::is_same<T, bool>())
                expectedTypeName = "Boolean";

            validationErrorMessage << '\t' << fmt::format("{0} must be of type {1}", a_paramName, expectedTypeName);
            return;
        }

        r_paramReference = parameter.value();
    };

    // TODO: Add Parameter Names
    setParamValue(qchemConfig.RunFolder, "");
    setParamValue(qchemConfig.GeometryInput, "");

    setParamValue(qchemConfig.NumCPUs, "", std::optional<int>(MAX_CORES));

    setParamValue(qchemConfig.SpinFlip, "");
    setParamValue(qchemConfig.SCFAlgorithm, "");
    setParamValue(qchemConfig.Theory, "");
    setParamValue(qchemConfig.Guess, "");

    // Fail if Validation Errors Occurred
    if (!validationErrorMessage.str().empty()) {
        std::cerr << fmt::format("ERROR: Failed to Parse {0}", a_filePath) << std::endl;
        std::cerr << validationErrorMessage.str() << std::endl;

        return {};
    }

    // Return Created Object
    return qchemConfig;
}

}