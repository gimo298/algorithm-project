#include "Config.h"

#include "Utils/GenerateInput.h"

#include "Mapping/BWT.h"
#include "Mapping/IndexMapping.h"
#include "Mapping/KMP.h"
#include "Mapping/RabinKarp.h"
#include "Mapping/TrivialMapping.h"

#include "Assembly/Reconstruct.h"
#include "Compare/CompareResult.h"
#include "Utils/IO.h"
#include "Time/executeTime.h"

#include <string>
#include <vector>

using namespace std;

int main() {

    Config cfg;
    cfg.length = 10000;
    cfg.CntOfReads = 1000;
    //cfg.LenOfReads = 15;
    cfg.ErrorRate = 0.01;
    cfg.allowedMismatch = 3;

    fs::path inputDir =
        generateInputFiles(cfg);

    string genome =
        loadGenome(inputDir / "DNA.txt");

    vector<string> shortReads =
        loadShortReads(inputDir / "ShortReads.txt");

    vector<info> trivialResults =
        trivialMapping(
            genome,
            shortReads,
            cfg.allowedMismatch
        );

    vector<info> bwtResults =
        BWTMapping(
            genome,
            shortReads,
            cfg.allowedMismatch
        );

    vector<info> indexResults =
        indexMapping(
            genome,
            shortReads,
            cfg.allowedMismatch
        );

    vector<info> rabinKarpResults =
        RabinKarpMapping(
            genome,
            shortReads,
            cfg.allowedMismatch
        );

    vector<info> kmpResults =
        KMPMapping(
            genome,
            shortReads,
            cfg.allowedMismatch
        );

    // saveResults(
    //     "2_TrivialMapping_result.txt",
    //     trivialResults
    // );

    // saveResults(
    //     "3_BWT_result.txt",
    //     bwtResults
    // );

    // saveResults(
    //     "4_RabinKarpMapping_result.txt",
    //     rabinKarpResults
    // );

    // saveResults(
    //     "5_IndexMapping_result.txt",
    //     indexResults
    // );

    // saveResults(
    //     "6_KMPMapping_result.txt",
    //     kmpResults
    // );

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

    compareReconstructionWithOriginal(
        "Trivial",
        genome,
        reconstructedTrivial
    );

    compareReconstructionWithOriginal(
        "BWT",
        genome,
        reconstructedBWT
    );

    compareReconstructionWithOriginal(
        "RabinKarp",
        genome,
        reconstructedRabinKarp
    );

    compareReconstructionWithOriginal(
        "IndexMapping",
        genome,
        reconstructedIndexMapping
    );

    compareReconstructionWithOriginal(
        "KMP",
        genome,
        reconstructedKMP
    );

    measureAndSaveExecutionTimes(
        cfg,
        genome,
        shortReads
    );

    return 0;
}