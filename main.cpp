#include "Config.h"

#include "Result/RunMappingExperiment.h"
#include "Utils/GenerateInput.h"
#include "Utils/ResultPaths.h"

#include <string>
#include <vector>

using namespace std;

int main() {

    int version = 3; // 사용하는 DNA의 version
    int runCount = 3; // read 길이별 실행횟수

    Config cfg;
    cfg.length = 10000;
    cfg.CntOfReads = 1000;
    cfg.ErrorRate = 0.01;
    cfg.allowedMismatch = 3;

    vector<int> readLengths = {
        15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100
    }; // 실험해볼 read 길이의 목록

    ensureInputVersionDirectory(version);
    ensureGenomeFile(cfg, version);

    for (int readLength : readLengths) {
        ensureShortReadsDirectory(
            version,
            readLength
        );

        for (int runIndex = 1;
             runIndex <= runCount;
             ++runIndex) {
            filesystem::path genomePath =
                getGenomeFilePath(version);

            filesystem::path shortReadsPath =
                ensureShortReadsFile(
                    cfg,
                    version,
                    readLength,
                    runIndex
                );

            filesystem::path outputPath =
                getMappingResultJsonPath(
                    version,
                    readLength,
                    runIndex
                );

            runMappingExperiment(
                cfg,
                version,
                readLength,
                runIndex,
                genomePath,
                shortReadsPath,
                outputPath
            );
        }
    }

    return 0;
}