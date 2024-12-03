#pragma once

#include <string>
#include <optional>

struct QChemConfig
{
public:
    std::string GeometryInput;

    int NumCPUs;

    bool SpinFlip;
    std::string SCFAlgorithm;
    int Theory;
    bool Guess;

public:
    bool AreValuesValid() const;

    // Builder Methods
    static std::optional<QChemConfig> CreateFromTOMLFile(std::string a_filePath);
};