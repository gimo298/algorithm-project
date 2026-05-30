#pragma once

#include "../Config.h"
#include "../Utils/IO.h"

#include <string>
#include <vector>

using namespace std;

string reconstructFromMappingResults(
    const vector<info>& mappingResults,
    const vector<string>& shortReads,
    const Config& cfg
);

void saveReconstructedSequence(
    const string& outputPath,
    const string& sequence
);

string runReconstruction(
    const string& algorithmName,
    const vector<info>& mappingResults,
    const vector<string>& shortReads,
    const Config& cfg
);
