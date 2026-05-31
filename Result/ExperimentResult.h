#pragma once

#include "../Config.h"

#include <filesystem>
#include <string>
#include <vector>
#include <map>

using namespace std;

struct CompareStats {
    string algorithmName;
    int originalLength;
    int reconstructedLength;
    int comparedLength;
    int matchCount;
    int mismatchCount;
    int unknownCount;
    int lengthDifference;
    double accuracy;
};

struct ExecutionStats {
    string algorithmName;
    double executionTimeMs;
};

CompareStats calculateCompareStats(
    const string& algorithmName,
    const string& original,
    const string& reconstructed
);

void saveExperimentResultJson(
    const Config& cfg,
    const vector<CompareStats>& compareStatsList,
    const vector<ExecutionStats>& executionStatsList
);

void saveExperimentResultJson(
    const Config& cfg,
    const vector<CompareStats>& compareStatsList,
    const vector<ExecutionStats>& executionStatsList,
    int version,
    const std::filesystem::path& outputPath
);