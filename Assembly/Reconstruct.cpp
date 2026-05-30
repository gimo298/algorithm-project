#include "Reconstruct.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

int baseToScoreIndex(char base) {
    switch (base) {
        case 'A': return 0;
        case 'C': return 1;
        case 'G': return 2;
        case 'T': return 3;
        default:  return -1;
    }
}

char scoreIndexToBase(int index) {
    switch (index) {
        case 0: return 'A';
        case 1: return 'C';
        case 2: return 'G';
        case 3: return 'T';
        default: return 'N';
    }
}

int getMismatchWeight(int mismatch, int allowedMismatch) {
    if (mismatch < 0) {
        return 0;
    }

    int weight = allowedMismatch - mismatch + 1;

    if (weight < 1) {
        weight = 1;
    }

    return weight;
}

string reconstructFromMappingResults(
    const vector<info>& mappingResults,
    const vector<string>& shortReads,
    const Config& cfg
) {
    int genomeLength = cfg.length;

    if (genomeLength <= 0) {
        throw runtime_error("genome length는 0보다 커야 합니다.");
    }

    if (mappingResults.size() != shortReads.size()) {
        throw runtime_error(
            "mappingResults 개수와 shortReads 개수가 일치하지 않습니다."
        );
    }

    vector<vector<int>> score(
        genomeLength,
        vector<int>(4, 0)
    );

    vector<int> coverage(genomeLength, 0);

    for (size_t i = 0; i < shortReads.size(); i++) {

        const info& mapInfo = mappingResults[i];
        const string& read = shortReads[i];

        if (mapInfo.position < 0) {
            continue;
        }

        int start = mapInfo.position;
        int weight =
            getMismatchWeight(
                mapInfo.mismatch,
                cfg.allowedMismatch
            );

        for (int j = 0; j < (int)read.size(); j++) {

            int pos = start + j;

            if (pos < 0 || pos >= genomeLength) {
                continue;
            }

            int baseIndex =
                baseToScoreIndex(read[j]);

            if (baseIndex == -1) {
                continue;
            }

            score[pos][baseIndex] += weight;
            coverage[pos]++;
        }
    }

    string reconstructed;
    reconstructed.reserve(genomeLength);

    for (int pos = 0; pos < genomeLength; pos++) {

        if (coverage[pos] == 0) {
            reconstructed += 'N';
            continue;
        }

        int bestIndex = 0;
        int bestScore = score[pos][0];
        bool tie = false;

        for (int b = 1; b < 4; b++) {

            if (score[pos][b] > bestScore) {
                bestScore = score[pos][b];
                bestIndex = b;
                tie = false;
            }
            else if (score[pos][b] == bestScore &&
                     bestScore > 0) {
                tie = true;
            }
        }

        if (tie) {
            reconstructed += 'N';
        }
        else {
            reconstructed += scoreIndexToBase(bestIndex);
        }
    }

    return reconstructed;
}

void saveReconstructedSequence(
    const string& outputPath,
    const string& sequence
) {
    ofstream fout(outputPath);

    if (!fout) {
        throw runtime_error(
            outputPath + " 파일을 생성할 수 없습니다."
        );
    }

    fout << sequence << '\n';
}

string runReconstruction(
    const string& algorithmName,
    const vector<info>& mappingResults,
    const vector<string>& shortReads,
    const Config& cfg
) {
    string reconstructed =
        reconstructFromMappingResults(
            mappingResults,
            shortReads,
            cfg
        );

    string outputPath =
        "7_Reconstruct_" + algorithmName + ".txt";

    saveReconstructedSequence(
        outputPath,
        reconstructed
    );

    cout << outputPath << " 저장 완료\n";

    return reconstructed;
}
