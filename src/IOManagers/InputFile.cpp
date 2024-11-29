#include "InputFile.hpp"

#include <iostream>
#include <sstream>

#include <exception>

#include <fmt/format.h>
#include <toml++/toml.hpp>

/// @brief 
/// @param a_fileName 
InputFile::InputFile(std::string a_fileName)
{
    std::string filePath = fmt::format("{0}/Molecule-Tests/{1}", ASSETS_PATH, a_fileName);

    // Parse the TOML File
    toml::v3::ex::parse_result parsedFile;
    try {
        parsedFile = toml::parse_file(filePath); 
    }
    catch(const toml::parse_error& e) {
        std::stringstream errorMessage;
        errorMessage << fmt::format("ERROR: Failed to Parse {0}", a_fileName) << '\n';
        errorMessage << '\t' << e.what();

        throw std::runtime_error(errorMessage.str());
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

        validationErrors << '\t' << fmt::format("{0} must be of type {1}", a_argumentName, expectedTypeName) << '\n';
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
        std::stringstream errorMessage;
        errorMessage << fmt::format("ERROR: Failed to Parse {0}", a_fileName) << '\n';
        errorMessage << validationErrors.str();

        throw std::runtime_error(errorMessage.str());
    }

    // Set Class Member Values
    _runFolder = parsedFile["HPCSetup"]["RunFolder"].value_or("default");

    _numCPUs = parsedFile["HPCSetup"]["NumCPUs"].value_or(MAX_CORES);
    _useGPU = parsedFile["HPCSetup"]["UseGPU"].value_or(false);

    _maxHours = parsedFile["HPCSetup"]["MaxHours"].value_or(48.0);

    _moleculeName = parsedFile["Molecule"]["Name"].value_or("");

    _createGeometry = parsedFile["Molecule"]["CreateGeometry"].value_or(true);
    _geometryStartIndex = parsedFile["Molecule"]["GeometryStartingIndex"].value_or(0);

    _method = parsedFile["Computation"]["Method"].value_or("");

    _multiplicity = parsedFile["Computation"]["Multiplicity"].value_or(0);
    _temperature = parsedFile["Computation"]["Temerature"].value_or(0.0);
    _timestepDuration = parsedFile["Computation"]["TimestepDuration"].value_or(0.0);

    _numBranches = parsedFile["Computation"]["NumBranches"].value_or(0);
    _numStates = parsedFile["Computation"]["NumStates"].value_or(0);
    _numTrajectories = parsedFile["Computation"]["NumTrajectories"].value_or(0);
    _numTimesteps = parsedFile["Computation"]["NumTimesteps"].value_or(0);
}

/// @brief 
InputFile::~InputFile() { }

/// @brief Return the Full path of the Run Folder
std::string InputFile::RunFolderPath() const
{ 
    return fmt::format("{0}/{1}", RESULTS_PATH, _runFolder);
}