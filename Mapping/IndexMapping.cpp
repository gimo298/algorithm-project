#include "IndexMapping.h"
#include "../Utils/IO.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

// index 생성
unordered_map<string, vector<int>> createIndex(
    const string& genome,
    int readLength
) {
    unordered_map<string, vector<int>> index;

    if (readLength <= 0 ||
        readLength > static_cast<int>(genome.size())) {
        return index;
    }

    for (size_t i = 0;
         i <= genome.size() - readLength;
         i++) {

        string piece =
            genome.substr(i, readLength);

        index[piece].push_back(
            static_cast<int>(i)
        );
    }

    return index;
}

// Index Mapping
vector<info> indexMapping(
    const string& genome,
    const vector<string>& shortReads,
    int allowedMismatch
) {
    vector<info> results(shortReads.size());

    if (shortReads.empty()) {
        return results;
    }

    int readLength =
        static_cast<int>(shortReads[0].size());

    unordered_map<string, vector<int>> index =
        createIndex(genome, readLength);

    for (size_t i = 0;
         i < shortReads.size();
         i++) {

        const string& shortRead =
            shortReads[i];

        info result;
        result.position = -1;
        result.mismatch = -1;
        result.ambiguous = false;

        if (shortRead.size() > genome.size()) {
            results[i] = result;
            continue;
        }

        // 1. exact match 먼저 확인
        auto it = index.find(shortRead);

        if (it != index.end()) {

            const vector<int>& positions =
                it->second;

            result.position =
                positions[0];

            result.mismatch = 0;

            if (positions.size() >= 2) {
                result.ambiguous = true;
            }

            results[i] = result;
            continue;
        }

        // 2. exact match가 없을 때 mismatch 허용 탐색
        int bestMismatch =
            allowedMismatch + 1;

        int bestCount = 0;

        for (size_t j = 0;
             j <= genome.size() - shortRead.size();
             j++) {

            int cntOfMismatch = 0;

            for (size_t k = 0;
                 k < shortRead.size();
                 k++) {

                if (genome[j + k] != shortRead[k]) {
                    cntOfMismatch++;
                }

                if (cntOfMismatch > allowedMismatch) {
                    break;
                }
            }

            if (cntOfMismatch <= allowedMismatch) {

                if (cntOfMismatch < bestMismatch) {

                    bestMismatch = cntOfMismatch;
                    result.position =
                        static_cast<int>(j);
                    result.mismatch =
                        cntOfMismatch;
                    bestCount = 1;
                    result.ambiguous = false;
                }
                else if (cntOfMismatch == bestMismatch) {

                    bestCount++;
                    result.ambiguous = true;
                }
            }
        }

        results[i] = result;
    }

    return results;
}

void runIndexMappingFile(const Config& cfg) {

    int allowedMismatch =
        cfg.allowedMismatch;

    if (allowedMismatch < 0) {
        throw runtime_error(
            "허용 mismatch 수는 0 이상의 정수여야 합니다."
        );
    }

    string genome =
        loadGenome("0_DNA.txt");

    vector<string> shortReads =
        loadShortReads("1_ShortReads.txt");

    vector<info> results =
        indexMapping(
            genome,
            shortReads,
            allowedMismatch
        );

    saveResults(
        "5_IndexMapping_result.txt",
        results
    );

    cout << "5_IndexMapping_result.txt 저장 완료\n";
}