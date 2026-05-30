#include "TrivialMapping.h"

#include "../Utils/IO.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

vector<info> trivialMapping(
    const string& genome,
    const vector<string>& shortReads,
    int allowedMismatch
) {
    vector<info> results(shortReads.size());

    for (size_t i = 0; i < shortReads.size(); i++) {

        const string& shortRead = shortReads[i];

        info result;
        result.position = -1;
        result.mismatch = -1;
        result.ambiguous = false;

        if (shortRead.size() > genome.size()) {
            results[i] = result;
            continue;
        }

        for (size_t j = 0; j <= genome.size() - shortRead.size(); j++) {

            int cntOfMismatch = 0;

            for (size_t k = 0; k < shortRead.size(); k++) {

                if (genome[j + k] != shortRead[k]) {
                    cntOfMismatch++;
                }
            }

            if (cntOfMismatch <= allowedMismatch) {

                if (result.position != -1) {
                    result.ambiguous = true;
                }

                result.position = static_cast<int>(j);
                result.mismatch = cntOfMismatch;
            }
        }

        results[i] = result;
    }

    return results;
}

void runTrivialMappingFile(const Config& cfg) {

    int allowedMismatch = cfg.allowedMismatch;

    if (allowedMismatch < 0) {
        throw runtime_error("허용 mismatch 수는 0 이상의 정수여야 합니다.");
    }

    string genome =
        loadGenome("0_DNA.txt");

    vector<string> shortReads =
        loadShortReads("1_ShortReads.txt");

    vector<info> results =
        trivialMapping(genome, shortReads, allowedMismatch);

    saveResults(
        "2_TrivialMapping_result.txt",
        results
    );

    cout << "2_TrivialMapping_result.txt 저장 완료\n";
}