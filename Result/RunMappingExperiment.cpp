#include "RunMappingExperiment.h"

#include "../Assembly/Reconstruct.h"
#include "../Mapping/BWT.h"
#include "../Mapping/IndexMapping.h"
#include "../Mapping/KMP.h"
#include "../Mapping/RabinKarp.h"
#include "../Mapping/TrivialMapping.h"
#include "../Utils/IO.h"
#include "../Utils/ResultPaths.h"
#include "ExperimentResult.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace fs = std::filesystem;

template <typename Func>
vector<info> measureMappingTime(
    const string& algorithmName,
    Func mappingFunc,
    vector<ExecutionStats>& executionStatsList
) {
    auto start = chrono::high_resolution_clock::now();

    vector<info> result = mappingFunc();

    auto end = chrono::high_resolution_clock::now();

    double elapsedMs =
        chrono::duration<double, milli>(
            end - start
        ).count();

    executionStatsList.push_back({
        algorithmName,
        elapsedMs
    });

    return result;
}

void validateShortReadLength(
    const vector<string>& shortReads,
    int readLength
) {
    for (const string& shortRead : shortReads) {
        if (static_cast<int>(shortRead.size()) != readLength) {
            throw runtime_error(
                "ShortReads.txt의 read 길이가 요청한 readLength와 일치하지 않습니다."
            );
        }
    }
}

void runMappingExperiment(
    const Config& baseConfig,
    int inputVersion,
    int readLength,
    int runIndex,
    const fs::path& genomePath,
    const fs::path& shortReadsPath,
    const fs::path& outputPath
) {
    if (inputVersion <= 0) {
        throw runtime_error("inputVersion은 1 이상의 정수여야 합니다.");
    }

    if (readLength <= 0) {
        throw runtime_error("readLength는 1 이상의 정수여야 합니다.");
    }

    if (runIndex <= 0) {
        throw runtime_error("runIndex는 1 이상의 정수여야 합니다.");
    }

    Config cfg = baseConfig;
    cfg.LenOfReads = readLength;

    if (fs::exists(outputPath)) {
        cout << outputPath.string() << " 파일이 이미 존재하여 생략합니다.\n";
        return;
    }

    if (!fs::exists(genomePath)) {
        throw runtime_error(genomePath.string() + " 파일이 존재하지 않습니다.");
    }

    if (!fs::exists(shortReadsPath)) {
        throw runtime_error(shortReadsPath.string() + " 파일이 존재하지 않습니다.");
    }

    string genome = loadGenome(genomePath);
    vector<string> shortReads = loadShortReads(shortReadsPath);

    validateShortReadLength(shortReads, readLength);

    vector<ExecutionStats> executionStatsList;
    vector<CompareStats> compareStatsList;

    vector<info> trivialResults =
        measureMappingTime(
            "Trivial",
            [&]() {
                return trivialMapping(
                    genome,
                    shortReads,
                    cfg.allowedMismatch
                );
            },
            executionStatsList
        );

    vector<info> bwtResults =
        measureMappingTime(
            "BWT",
            [&]() {
                return BWTMapping(
                    genome,
                    shortReads,
                    cfg.allowedMismatch
                );
            },
            executionStatsList
        );

    vector<info> rabinKarpResults =
        measureMappingTime(
            "RabinKarp",
            [&]() {
                return RabinKarpMapping(
                    genome,
                    shortReads,
                    cfg.allowedMismatch
                );
            },
            executionStatsList
        );

    vector<info> indexResults =
        measureMappingTime(
            "IndexMapping",
            [&]() {
                return indexMapping(
                    genome,
                    shortReads,
                    cfg.allowedMismatch
                );
            },
            executionStatsList
        );

    vector<info> kmpResults =
        measureMappingTime(
            "KMP",
            [&]() {
                return KMPMapping(
                    genome,
                    shortReads,
                    cfg.allowedMismatch
                );
            },
            executionStatsList
        );

    string reconstructedTrivial =
        reconstructFromMappingResults(
            trivialResults,
            shortReads,
            cfg
        );

    string reconstructedBWT =
        reconstructFromMappingResults(
            bwtResults,
            shortReads,
            cfg
        );

    string reconstructedRabinKarp =
        reconstructFromMappingResults(
            rabinKarpResults,
            shortReads,
            cfg
        );

    string reconstructedIndexMapping =
        reconstructFromMappingResults(
            indexResults,
            shortReads,
            cfg
        );

    string reconstructedKMP =
        reconstructFromMappingResults(
            kmpResults,
            shortReads,
            cfg
        );

    compareStatsList.push_back(
        calculateCompareStats(
            "Trivial",
            genome,
            reconstructedTrivial
        )
    );

    compareStatsList.push_back(
        calculateCompareStats(
            "BWT",
            genome,
            reconstructedBWT
        )
    );

    compareStatsList.push_back(
        calculateCompareStats(
            "RabinKarp",
            genome,
            reconstructedRabinKarp
        )
    );

    compareStatsList.push_back(
        calculateCompareStats(
            "IndexMapping",
            genome,
            reconstructedIndexMapping
        )
    );

    compareStatsList.push_back(
        calculateCompareStats(
            "KMP",
            genome,
            reconstructedKMP
        )
    );

    saveExperimentResultJson(
        cfg,
        compareStatsList,
        executionStatsList,
        inputVersion,
        outputPath
    );
}