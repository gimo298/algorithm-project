#pragma once

#include "../Config.h"
#include "../Utils/IO.h"
#include <string>
#include <vector>
using namespace std;

void runBWTMappingFile(const Config& cfg);

vector<info> BWTMapping(
    const string& genome,
    const vector<string>& shortReads,
    int allowedMismatch
);