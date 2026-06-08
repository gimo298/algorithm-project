#include "Config.h"

#include "Result/RunMappingExperiment.h"
#include "Utils/GenerateInput.h"
#include "Utils/ResultPaths.h"

#include <atomic>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace std;

struct ExperimentTask {
    int readLength;
    int runIndex;
};

int main() {

    int version = 1;
    int runCount = 3;

    Config cfg;
    cfg.length = 10000;
    cfg.CntOfReads = 3000;
    cfg.ErrorRate = 0.01;
    cfg.allowedMismatch = 3;

    vector<int> readLengths = {
        50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200
    };

    // 동시에 실행할 스레드 수
    // 처음에는 2~4 정도 추천
    int threadCount = 2;

    ensureInputVersionDirectory(version);
    ensureGenomeFile(cfg, version);

    vector<ExperimentTask> tasks;

    for (int readLength : readLengths) {

        ensureShortReadsDirectory(
            version,
            readLength
        );

        for (int runIndex = 1;
             runIndex <= runCount;
             runIndex++) {

            tasks.push_back({
                readLength,
                runIndex
            });
        }
    }

    atomic<int> taskIndex = 0;
    mutex coutMutex;

    auto worker = [&]() {

        while (true) {

            int index = taskIndex++;

            if (index >= (int)tasks.size()) {
                break;
            }

            ExperimentTask task = tasks[index];

            try {
                Config localCfg = cfg;
                localCfg.LenOfReads = task.readLength;

                filesystem::path genomePath =
                    getGenomeFilePath(version);

                filesystem::path shortReadsPath =
                    ensureShortReadsFile(
                        localCfg,
                        version,
                        task.readLength,
                        task.runIndex
                    );

                filesystem::path outputPath =
                    getMappingResultJsonPath(
                        version,
                        task.readLength,
                        task.runIndex
                    );

                {
                    lock_guard<mutex> lock(coutMutex);
                    cout << "[START] readLength="
                         << task.readLength
                         << ", runIndex="
                         << task.runIndex
                         << '\n';
                }

                runMappingExperiment(
                    localCfg,
                    version,
                    task.readLength,
                    task.runIndex,
                    genomePath,
                    shortReadsPath,
                    outputPath
                );

                {
                    lock_guard<mutex> lock(coutMutex);
                    cout << "[DONE] readLength="
                         << task.readLength
                         << ", runIndex="
                         << task.runIndex
                         << '\n';
                }
            }
            catch (const exception& e) {

                lock_guard<mutex> lock(coutMutex);

                cerr << "[ERROR] readLength="
                     << task.readLength
                     << ", runIndex="
                     << task.runIndex
                     << " : "
                     << e.what()
                     << '\n';
            }
        }
    };

    vector<thread> workers;

    for (int i = 0; i < threadCount; i++) {
        workers.push_back(thread(worker));
    }

    for (thread& t : workers) {
        t.join();
    }

    cout << "전체 실험 완료\n";

    return 0;
}