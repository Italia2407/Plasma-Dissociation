#pragma once

#include "QChemConfig.hpp"

#include <string>

namespace QChemManagement {

bool CreateQChemInput(QChemConfig a_qchemConfig);
void RunQChem(std::string a_qchemInputFile, std::string a_qchemOutputFile, int a_numCPUs = MAX_CORES);

bool SubmitQChemJob(QChemConfig a_qchemConfig);

}