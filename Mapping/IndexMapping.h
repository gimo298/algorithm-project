#pragma once

#include "../Config.h"
#include "../Utils/IO.h"
#include <string>
#include <vector>
using namespace std;

void runIndexMappingFile(const Config& cfg);

vector<info> indexMapping(
    const string& genome,
    const vector<string>& shortReads,
    int allowedMismatch
);