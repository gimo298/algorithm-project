#pragma once

#include "../Config.h"
#include "../Utils/IO.h"
#include <string>
#include <vector>
using namespace std;

void runKMPMappingFile(const Config& cfg);

vector<info> KMPMapping(
    const string& genome,
    const vector<string>& shortReads,
    int allowedMismatch
);