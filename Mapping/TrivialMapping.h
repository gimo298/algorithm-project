#pragma once

#include "../Config.h"
#include "../Utils/IO.h"
#include <string>
#include <vector>
using namespace std;

void runTrivialMappingFile(const Config& cfg);

vector<info> trivialMapping(
    const string& genome,
    const vector<string>& shortReads,
    int allowedMismatch
);