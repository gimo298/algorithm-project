#ifndef IO_H
#define IO_H

#include <filesystem>
#include <string>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

struct info {
    int position;
    int mismatch;
    bool ambiguous;
};

string loadGenome(
    const fs::path& genomePath
);

vector<string> loadShortReads(
    const fs::path& shortReadsPath
);

void saveResults(
    const fs::path& resultPath,
    const vector<info>& results
);

#endif