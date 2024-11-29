#pragma once

#include <string>

class InputFile
{
public:
    InputFile(std::string a_fileName);
    ~InputFile();

private:
    // HPC Setup Parameters
    std::string _runFolder;

    int _numCPUs;
    bool _useGPU;

    double _maxHours;

    // Molecule Parameters
    std::string _moleculeName;

    bool _createGeometry;
    int _geometryStartIndex;

    // Computation Parameters
    std::string _method;

    int _multiplicity;
    double _temperature;
    double _timestepDuration;

    int _numBranches;
    int _numStates;
    int _numTrajectories;
    int _numTimesteps;

public:
    std::string RunFolderPath() const;
    std::string RunFolderName() const { return _runFolder; }

    int NumCPUs() const { return _numCPUs; }
    bool UseGPU() const { return _useGPU; }

    double MaxHours() const { return _maxHours; }

    std::string MoleculeName() const { return _moleculeName; }

    bool CreateGeometry() const { return _createGeometry; }
    int GeometryStartIndex() const { return _geometryStartIndex; }

    std::string Method() const { return _method; }

    int Multiplicity() const { return _multiplicity; }
    double Temperature() const { return _temperature; }
    double TimestepDuration() const { return _timestepDuration; }

    int NumBranches() const { return _numBranches; }
    int NumStates() const { return _numStates; }
    int NumTrajectories() const { return _numTrajectories; }
    int NumTimesteps() const { return _numTimesteps; }
};