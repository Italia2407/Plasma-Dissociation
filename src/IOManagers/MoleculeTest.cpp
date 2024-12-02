#include "MoleculeTest.hpp"

#include <iostream>
#include <sstream>

#include <exception>

#include <fmt/format.h>
#include <toml++/toml.hpp>

/// @brief Retrieve the Full Path of the RunFolder
std::string MoleculeTest::GetRunFolderPath() const
{
    return fmt::format("{0}/{1}", RESULTS_PATH, RunFolderName); 
}

/// @brief Check wether the Values of the Molecule Test are Valid
bool MoleculeTest::AreValuesValid() const
{
    // TODO: Implement actual Checks
    return true;
}

/// @brief 
/// @param a_fileName The Name of the TOML File to be Parsed
/// @return The Created MoleculeTest Object, if Successful
std::optional<MoleculeTest> MoleculeTest::CreateFromTOMLFile(std::string a_fileName)
{
    std::string filePath = fmt::format("{0}/Molecule-Tests/{1}", ASSETS_PATH, a_fileName);

    // Ensure TOML File Can be Parsed Correctly
    toml::v3::ex::parse_result parsedFile;
    try {
        parsedFile = toml::parse_file(filePath); 
    }
    catch (const toml::parse_error& e) {
        std::cerr << fmt::format("ERROR: Failed to Parse {0}", a_fileName) << std::endl;
        std::cerr << '\t' << e.what() << std::endl;

        return {};
    }

    // TODO: Argument is not Good name, find a more suitable one instead
    // Ensure Input Arguments are of the Correct Type
    bool argumentsValidated = true;
    std::stringstream validationErrors;

    auto validateArgumentType = [&](std::string a_argumentName, toml::v3::node_type a_expectedType, bool a_optional = false) {
        auto argumentType = parsedFile.at_path(a_argumentName).type();

        if (argumentType == a_expectedType)
            return;
        // Extra case for Generic Number
        else if (argumentType == toml::v3::node_type::integer && a_expectedType == toml::v3::node_type::floating_point)
            return;
        // Extra case if Value is not Present
        else if (a_optional && argumentType == toml::v3::node_type::none)
            return;

        // Argument is not of Expected Type
        std::string expectedTypeName = "";
        if (a_expectedType == toml::v3::node_type::string)
            expectedTypeName = "String";
        else if (a_expectedType == toml::v3::node_type::integer)
            expectedTypeName = "Integer";
        else if (a_expectedType == toml::v3::node_type::floating_point)
            expectedTypeName = "Number";
        else if (a_expectedType == toml::v3::node_type::boolean)
            expectedTypeName = "Boolean";

        // Only Add New Line if not First Failed Validation
        if (!argumentsValidated)
            validationErrors << '\n';

        validationErrors << '\t' << fmt::format("{0} must be of type {1}", a_argumentName, expectedTypeName);
        argumentsValidated = false;
    };

    validateArgumentType("HPCSetup.RunFolder", toml::v3::node_type::string);

    validateArgumentType("HPCSetup.NumCPUs", toml::v3::node_type::integer, true);
    validateArgumentType("HPCSetup.UseGPU", toml::v3::node_type::boolean, true);

    validateArgumentType("HPCSetup.MaxHours", toml::v3::node_type::floating_point, true);

    validateArgumentType("Molecule.Name", toml::v3::node_type::string);

    validateArgumentType("Molecule.CreateGeometry", toml::v3::node_type::boolean, true);
    validateArgumentType("Molecule.GeometryStartingIndex", toml::v3::node_type::integer, true);

    validateArgumentType("Computation.Method", toml::v3::node_type::string);

    validateArgumentType("Computation.Multiplicity", toml::v3::node_type::integer);
    validateArgumentType("Computation.Temperature", toml::v3::node_type::floating_point);
    validateArgumentType("Computation.TimestepDuration", toml::v3::node_type::floating_point);

    validateArgumentType("Computation.NumBranches", toml::v3::node_type::integer);
    validateArgumentType("Computation.NumStates", toml::v3::node_type::integer);
    validateArgumentType("Computation.NumTrajectories", toml::v3::node_type::integer);
    validateArgumentType("Computation.NumTimesteps", toml::v3::node_type::integer);

    if (!argumentsValidated) {
        std::cerr << fmt::format("ERROR: Failed to Parse {0}", a_fileName) << std::endl;
        std::cerr << validationErrors.str() << std::endl;

        return {};
    }

    // Fill in Struct's Member Values
    MoleculeTest moleculeTest; {
        moleculeTest.RunFolderName = parsedFile["HPCSetup"]["RunFolder"].value_or("default");

        moleculeTest.NumCPUs = parsedFile["HPCSetup"]["NumCPUs"].value_or(MAX_CORES);
        moleculeTest.UseGPU = parsedFile["HPCSetup"]["UseGPU"].value_or(false);

        moleculeTest.MaxHours = parsedFile["HPCSetup"]["MaxHours"].value_or(48.0);

        moleculeTest.MoleculeName = parsedFile["Molecule"]["Name"].value_or("");

        moleculeTest.CreateGeometry = parsedFile["Molecule"]["CreateGeometry"].value_or(true);
        moleculeTest.GeometryStartIndex = parsedFile["Molecule"]["GeometryStartingIndex"].value_or(0);

        moleculeTest.Method = parsedFile["Computation"]["Method"].value_or("");

        moleculeTest.Multiplicity = parsedFile["Computation"]["Multiplicity"].value_or(0);
        moleculeTest.Temperature = parsedFile["Computation"]["Temerature"].value_or(0.0);
        moleculeTest.TimestepDuration = parsedFile["Computation"]["TimestepDuration"].value_or(0.0);

        moleculeTest.NumBranches = parsedFile["Computation"]["NumBranches"].value_or(0);
        moleculeTest.NumStates = parsedFile["Computation"]["NumStates"].value_or(0);
        moleculeTest.NumTrajectories = parsedFile["Computation"]["NumTrajectories"].value_or(0);
        moleculeTest.NumTimesteps = parsedFile["Computation"]["NumTimesteps"].value_or(0);
    }

    // Return Created Object
    return moleculeTest;
}