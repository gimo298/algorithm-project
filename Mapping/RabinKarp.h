#pragma once

#include "../Config.h"
#include "../Utils/IO.h"
#include <string>
#include <vector>
using namespace std;

void runRabinKarpMappingFile(const Config& cfg);

vector<info> RabinKarpMapping(
    const string& genome,
    const vector<string>& shortReads,
    int allowedMismatch
);