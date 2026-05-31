#include "ExperimentResult.h"

#include "../Utils/ResultPaths.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

CompareStats calculateCompareStats(
    const string& algorithmName,
    const string& original,
    const string& reconstructed
) {
    CompareStats stats;

    stats.algorithmName = algorithmName;
    stats.originalLength = original.size();
    stats.reconstructedLength = reconstructed.size();
    stats.comparedLength =
        min(stats.originalLength, stats.reconstructedLength);

    stats.matchCount = 0;
    stats.mismatchCount = 0;
    stats.unknownCount = 0;

    for (int i = 0; i < stats.comparedLength; i++) {
        if (reconstructed[i] == 'N') {
            stats.unknownCount++;
        }
        else if (original[i] == reconstructed[i]) {
            stats.matchCount++;
        }
        else {
            stats.mismatchCount++;
        }
    }

    stats.lengthDifference =
        abs(stats.originalLength - stats.reconstructedLength);

    stats.accuracy = 0.0;

    if (stats.originalLength > 0) {
        stats.accuracy =
            (double)stats.matchCount
            / stats.originalLength
            * 100.0;
    }

    return stats;
}

double findExecutionTime(
    const vector<ExecutionStats>& executionStatsList,
    const string& algorithmName
) {
    for (const auto& stat : executionStatsList) {
        if (stat.algorithmName == algorithmName) {
            return stat.executionTimeMs;
        }
    }

    return -1.0;
}

void saveExperimentResultJson(
    const Config& cfg,
    const vector<CompareStats>& compareStatsList,
    const vector<ExecutionStats>& executionStatsList,
    int version,
    const fs::path& outputPath
) {
    fs::create_directories(getInputVersionDir(version));
    fs::create_directories(outputPath.parent_path());

    if (fs::exists(outputPath)) {
        cout << cfg.LenOfReads
             << "길이의 json 파일이 이미 존재합니다.\n";
        return;
    }

    ofstream fout(outputPath);

    if (!fout) {
        throw runtime_error(outputPath.string() + " 파일 생성 실패");
    }

    fout << fixed << setprecision(2);

    fout << "{\n";
    fout << "  \"experiment_info\": {\n";
    fout << "    \"version\": " << version << ",\n";
    fout << "    \"genome_length\": " << cfg.length << ",\n";
    fout << "    \"reads_count\": " << cfg.CntOfReads << ",\n";
    fout << "    \"reads_length\": " << cfg.LenOfReads << ",\n";
    fout << "    \"error_rate\": " << cfg.ErrorRate << ",\n";
    fout << "    \"allowed_mismatch\": " << cfg.allowedMismatch << "\n";
    fout << "  },\n";

    fout << "  \"algorithms\": {\n";

    for (size_t i = 0; i < compareStatsList.size(); i++) {
        const CompareStats& stat = compareStatsList[i];

        double executionTime =
            findExecutionTime(
                executionStatsList,
                stat.algorithmName
            );

        fout << "    \"" << stat.algorithmName << "\": {\n";

        fout << "      \"execution_time_ms\": "
             << executionTime << ",\n";

        fout << "      \"accuracy\": "
             << stat.accuracy << ",\n";

        fout << "      \"match_count\": "
             << stat.matchCount << ",\n";

        fout << "      \"mismatch_count\": "
             << stat.mismatchCount << ",\n";

        fout << "      \"unknown_count_N\": "
             << stat.unknownCount << ",\n";

        fout << "      \"original_length\": "
             << stat.originalLength << ",\n";

        fout << "      \"reconstructed_length\": "
             << stat.reconstructedLength << ",\n";

        fout << "      \"length_difference\": "
             << stat.lengthDifference << "\n";

        fout << "    }";

        if (i + 1 < compareStatsList.size()) {
            fout << ",";
        }

        fout << "\n";
    }

    fout << "  }\n";
    fout << "}\n";

    fout.close();

    cout << outputPath.string() << " 저장 완료\n";
}