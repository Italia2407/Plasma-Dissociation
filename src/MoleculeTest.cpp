#include "MoleculeTest.hpp"

#include "IOManagement/TOMLReader.hpp"

#include <iostream>
#include <sstream>

#include <type_traits>

#include <fmt/format.h>

/// @brief Check whether the Values of the Molecule Test are Valid
bool MoleculeTest::AreValuesValid() const
{
    // TODO: Implement actual Checks
    return true;
}

/// @brief Create MoleculeTest Object from TOML File
/// @param a_fileName The TOML File to be Parsed
/// @return The Created MoleculeTest Object, if Successful
std::optional<MoleculeTest> MoleculeTest::CreateFromTOMLFile(std::string a_filePath)
{
    // Ensure TOML File was Parsed Correctly
    auto tomlReader = IOManagement::TOMLReader::ParseTOMLFile(a_filePath);
    if (!tomlReader)
        return {};

    // Create MoleculeTest Object, Ensuring Parameters are all Valid
    MoleculeTest moleculeTest;
    std::stringstream validationErrorMessage;

    // TODO: Not available prior to C++20
    auto setParamValue = [&]<typename T>(T& r_paramReference, std::string a_paramName, std::optional<T> a_defaultValue = {}) {
        auto parameter = tomlReader.value().GetParameter<T>(a_paramName, a_defaultValue);

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

    setParamValue(moleculeTest.RunFolder, "HPCSetup.RunFolder");

    setParamValue(moleculeTest.NumCPUs, "HPCSetup.NumCPUs", std::optional<int>(MAX_CORES));
    setParamValue(moleculeTest.UseGPU, "HPCSetup.UseGPU", std::optional<bool>(false));

    setParamValue(moleculeTest.MaxHours, "HPCSetup.MaxHours", std::optional<double>(48.0));

    setParamValue(moleculeTest.MoleculeName, "Molecule.Name");

    setParamValue(moleculeTest.CreateGeometry, "Molecule.CreateGeometry", std::optional<bool>(true));
    setParamValue(moleculeTest.GeometryStartIndex, "Molecule.GeometryStartingIndex", std::optional<int>(0));

    setParamValue(moleculeTest.Method, "Computation.Method");

    setParamValue(moleculeTest.Multiplicity, "Computation.Multiplicity");
    setParamValue(moleculeTest.Temperature, "Computation.Temperature");
    setParamValue(moleculeTest.TimestepDuration, "Computation.TimestepDuration");

    setParamValue(moleculeTest.NumBranches, "Computation.NumBranches");
    setParamValue(moleculeTest.NumStates, "Computation.NumStates");
    setParamValue(moleculeTest.NumTrajectories, "Computation.NumTrajectories");
    setParamValue(moleculeTest.NumTimesteps, "Computation.NumTimesteps");

    // Fail if Validation Errors Occurred
    if (!validationErrorMessage.str().empty()) {
        std::cerr << fmt::format("ERROR: Failed to Parse {0}", a_filePath) << std::endl;
        std::cerr << validationErrorMessage.str() << std::endl;

        return {};
    }

    // Return the Created Object
    return moleculeTest;
}
