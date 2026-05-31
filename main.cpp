#include "Config.h"

#include "DNA/GenerateDNA.h"
#include "Reads/GenerateShortReads.h"

#include "Mapping/BWT.h"
#include "Mapping/IndexMapping.h"
#include "Mapping/KMP.h"
#include "Mapping/RabinKarp.h"
#include "Mapping/TrivialMapping.h"

#include "Assembly/Reconstruct.h"
#include "Utils/IO.h"
#include "Result/ExperimentResult.h"

#include <chrono>
#include <string>
#include <vector>

using namespace std;

template <typename Func>
vector<info> measureMappingTime(
    const string& algorithmName,
    Func mappingFunc,
    vector<ExecutionStats>& executionStatsList
) {
    auto start =
        chrono::high_resolution_clock::now();

    vector<info> result =
        mappingFunc();

    auto end =
        chrono::high_resolution_clock::now();

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

int main() {

    Config cfg;
    cfg.length = 100000;
    cfg.CntOfReads = 10000;
    cfg.LenOfReads = 15;
    cfg.ErrorRate = 0.01;
    cfg.allowedMismatch = 1;

    generateDNAFile(cfg);
    generateShortReadsFile(cfg);

    string genome =
        loadGenome("0_DNA.txt");

    vector<string> shortReads =
        loadShortReads("1_ShortReads.txt");

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

    saveResults("2_TrivialMapping_result.txt", trivialResults);
    saveResults("3_BWT_result.txt", bwtResults);
    saveResults("4_RabinKarpMapping_result.txt", rabinKarpResults);
    saveResults("5_IndexMapping_result.txt", indexResults);
    saveResults("6_KMPMapping_result.txt", kmpResults);

    string reconstructedTrivial =
        runReconstruction(
            "Trivial",
            trivialResults,
            shortReads,
            cfg
        );

    string reconstructedBWT =
        runReconstruction(
            "BWT",
            bwtResults,
            shortReads,
            cfg
        );

    string reconstructedRabinKarp =
        runReconstruction(
            "RabinKarp",
            rabinKarpResults,
            shortReads,
            cfg
        );

    string reconstructedIndexMapping =
        runReconstruction(
            "IndexMapping",
            indexResults,
            shortReads,
            cfg
        );

    string reconstructedKMP =
        runReconstruction(
            "KMP",
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
        executionStatsList
    );

    return 0;
}