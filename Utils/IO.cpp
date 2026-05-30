#include "IO.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

using namespace std;

string loadGenome(
    const fs::path& genomePath
) {
    ifstream fin(genomePath);

    if (!fin) {
        throw runtime_error(
            genomePath.string() + " 파일을 열 수 없습니다."
        );
    }

    string genome;
    string line;

    while (getline(fin, line)) {
        genome += line;
    }

    if (genome.empty()) {
        throw runtime_error(
            genomePath.string() + " 파일이 비어 있습니다."
        );
    }

    return genome;
}

vector<string> loadShortReads(
    const fs::path& shortReadsPath
) {
    ifstream fin(shortReadsPath);

    if (!fin) {
        throw runtime_error(
            shortReadsPath.string() + " 파일을 열 수 없습니다."
        );
    }

    vector<string> shortReads;
    string shortRead;

    while (getline(fin, shortRead)) {
        if (!shortRead.empty()) {
            shortReads.push_back(shortRead);
        }
    }

    if (shortReads.empty()) {
        throw runtime_error(
            shortReadsPath.string() + " 파일이 비어 있습니다."
        );
    }

    return shortReads;
}

void saveResults(
    const fs::path& resultPath,
    const vector<info>& results
) {
    ofstream fout(resultPath);

    if (!fout) {
        throw runtime_error(
            resultPath.string() + " 파일을 생성할 수 없습니다."
        );
    }

    fout << left
         << setw(10) << "Read"
         << setw(15) << "Position"
         << setw(15) << "Mismatch"
         << setw(15) << "Ambiguous"
         << '\n';

    fout << string(55, '-') << '\n';

    for (size_t i = 0; i < results.size(); i++) {
        fout << left
             << setw(10) << i + 1
             << setw(15) << results[i].position
             << setw(15) << results[i].mismatch
             << setw(15)
             << (results[i].ambiguous ? "true" : "false")
             << '\n';
    }
}