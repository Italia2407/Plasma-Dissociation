#include "InputParser.hpp"

#include <fmt/format.h>
#include <toml++/toml.hpp>

#include <iostream>

/// @brief 
/// @param a_countArguments 
/// @param a_arguments 
InputParser::InputParser(int a_countArguments, char *a_arguments[]) :
    _optionsDescription("Program Options"), _positionalOptionsDescription(), _variableMap(), _inputFiles()
{
    // Create Command Line Options
    _optionsDescription.add_options()
        ("help", "Produce a Help Message")
        ("input-files", po::value<std::vector<std::string>>(), "Tested Input Files");

    // Create Positional Options
    _positionalOptionsDescription.add("input-files", -1);

    // Create Variable Map
    po::store(po::command_line_parser(a_countArguments, a_arguments).options(_optionsDescription).positional(_positionalOptionsDescription).run(), _variableMap);
    po::notify(_variableMap);
}

/// @brief 
InputParser::~InputParser() { }

/// @brief Print the Program's CLI Help Message.
/// @return Whether the Help Message was Printed.
bool InputParser::PrintHelpMessage()
{
    // Check if help Parameter was Passed
    if (!_variableMap.count("help"))
        return false;

    std::cout << _optionsDescription << std::endl;
    return true;
}

/// @brief 
/// @return Whether the Input Files were Parsed Successfully.
bool InputParser::ParseInputFiles()
{
    int numInputFiles = _variableMap.count("input-files");

    // Ensure at least 1 Input File has been Passed
    if (numInputFiles < 1) {
        std::cerr << "ERROR: At least one Input File is Required" << std::endl;
        return false;
    }
    std::vector<std::string> inputFileNames = _variableMap["input-files"].as<std::vector<std::string>>();
    std::vector<InputFile> inputFiles;

    for (int i = 0; i < numInputFiles; i++){
        std::string filePath = fmt::format("{0}/Molecule-Tests/{1}.toml", ASSETS_PATH, inputFileNames[i]);

        // Parse TOML File
        toml::v3::ex::parse_result parsedFile;
        try {
            parsedFile = toml::parse_file(filePath); 
        }
        catch(const toml::parse_error& error) {
            std::cerr << error.what() << std::endl;
            return false;
        }

        // Ensure Input Arguments are of the Correct Type
        bool parseSuccess = true;

        if (!parsedFile["HPCSetup"]["RunFolder"].is_string()){
            std::cerr << "ERROR: HPCSetup.RunFolder must be a String" << std::endl;
            parseSuccess = false;
        }

        if (!parsedFile["HPCSetup"]["NumCPUs"].is_integer() && !parsedFile["HPCSetup"]["NumCPUs"].is_value()){
            std::cerr << "ERROR: HPCSetup.NumCPUs must be an Integer" << std::endl;
            parseSuccess = false;
        }
        if (!parsedFile["HPCSetup"]["UseGPU"].is_boolean() && !parsedFile["HPCSetup"]["UseGPU"].is_value()){
            std::cerr << "ERROR: HPCSetup.UseGPU must be a Boolean" << std::endl;
            parseSuccess = false;
        }

        if (!parsedFile["HPCSetup"]["MaxHours"].is_number() && !parsedFile["HPCSetup"]["MaxHours"].is_value()){
            std::cerr << "ERROR: HPCSetup.MaxHours must be a Number" << std::endl;
            parseSuccess = false;
        }

        if (!parsedFile["Molecule"]["Name"].is_string()) {
            std::cerr << "ERROR: Molecule.Name must be a String" << std::endl;
            parseSuccess = false;
        }

        if (!parsedFile["Molecule"]["CreateGeometry"].is_boolean() && !parsedFile["Molecule"]["CreateGeometry"].is_value()){
            std::cerr << "ERROR: Molecule.CreateGeometry must be a Boolean" << std::endl;
            parseSuccess = false;
        }
        if (!parsedFile["Molecule"]["GeometryStartingIndex"].is_integer() && !parsedFile["Molecule"]["GeometryStartingIndex"].is_value()){
            std::cerr << "ERROR: Molecule.GeometryStartingIndex must be an Integer" << std::endl;
            parseSuccess = false;
        }

        if (!parsedFile["Computation"]["Method"].is_string()) {
            std::cerr << "ERROR: Computation.Method must be a String" << std::endl;
            parseSuccess = false;
        }

        if (!parsedFile["Computation"]["Multiplicity"].is_integer()) {
            std::cerr << "ERROR: Computation.Multiplicity must be an Integer" << std::endl;
            parseSuccess = false;
        }
        if (!parsedFile["Computation"]["Temperature"].is_number()) {
            std::cerr << "ERROR: Computation.Temperature must be a Number" << std::endl;
            parseSuccess = false;
        }
        if (!parsedFile["Computation"]["TimestepDuration"].is_number()) {
            std::cerr << "ERROR: Computation.TimestepDuration must be a Number" << std::endl;
            parseSuccess = false;
        }

        if (!parsedFile["Computation"]["NumBranches"].is_integer()) {
            std::cerr << "ERROR: Computation.NumBranches must be an Integer" << std::endl;
            parseSuccess = false;
        }
        if (!parsedFile["Computation"]["NumStates"].is_integer()) {
            std::cerr << "ERROR: Computation.NumStates must be an Integer" << std::endl;
            parseSuccess = false;
        }
        if (!parsedFile["Computation"]["NumTrajectories"].is_integer()) {
            std::cerr << "ERROR: Computation.NumTrajectories must be an Integer" << std::endl;
            parseSuccess = false;
        }
        if (!parsedFile["Computation"]["NumTimesteps"].is_integer()) {
            std::cerr << "ERROR: Computation.NumTimesteps must be an Integer" << std::endl;
            parseSuccess = false;
        }

        // Check if Parsing Succeeded
        if (!parseSuccess) {
            std::cerr << fmt::format("ERROR: Failed Parsing {0}", inputFileNames[i]) << std::endl;
            return false;
        }

        // Add Input File to List
        inputFiles.push_back(InputFile()); {
            inputFiles[i].FileName = inputFileNames[i];

            inputFiles[i].RunFolder = parsedFile["HPCSetup"]["RunFolder"].value_or("default");

            inputFiles[i].NumCPUs = parsedFile["HPCSetup"]["NumCPUs"].value_or(MAX_CORES);
            inputFiles[i].UseGPU = parsedFile["HPCSetup"]["UseGPU"].value_or(false);

            inputFiles[i].MaxHours = parsedFile["HPCSetup"]["MaxHours"].value_or(48.0);

            inputFiles[i].MoleculeName = parsedFile["Molecule"]["Name"].value_or("");

            inputFiles[i].CreateGeometry = parsedFile["Molecule"]["CreateGeometry"].value_or(true);
            inputFiles[i].GeometryStartIndex = parsedFile["Molecule"]["GeometryStartingIndex"].value_or(0);

            inputFiles[i].Method = parsedFile["Computation"]["Method"].value_or("");

            inputFiles[i].Multiplicity = parsedFile["Computation"]["Multiplicity"].value_or(0);
            inputFiles[i].Temperature = parsedFile["Computation"]["Temerature"].value_or(0.0);
            inputFiles[i].TimestepDuration = parsedFile["Computation"]["TimestepDuration"].value_or(0.0);

            inputFiles[i].NumBranches = parsedFile["Computation"]["NumBranches"].value_or(0);
            inputFiles[i].NumStates = parsedFile["Computation"]["NumStates"].value_or(0);
            inputFiles[i].NumTrajectories = parsedFile["Computation"]["NumTrajectories"].value_or(0);
            inputFiles[i].NumTimesteps = parsedFile["Computation"]["NumTimesteps"].value_or(0);
        }
    }

    // Clear Old Input Files
    _inputFiles.clear();
    
    // Copy Parsed Input Files into Storage
    for (auto inputFile : inputFiles)
        _inputFiles.push_back(inputFile);

    return true;
}

std::optional<InputFile> InputParser::GetInputFile(u_int32_t a_Index)
{
    // Ensure File Index Exists
    if (a_Index >= _inputFiles.size())
        return {};

    return _inputFiles[a_Index];
}
std::optional<InputFile> InputParser::GetInputFile(std::string a_FileName)
{
    for (int i = 0; i < _inputFiles.size(); i++) {
        if (_inputFiles[i].FileName == a_FileName)
            return _inputFiles[i];
    }

    // File Name Does not Exist
    return {};
}