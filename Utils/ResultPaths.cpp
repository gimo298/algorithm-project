#include "ResultPaths.h"

#include <filesystem>
#include <regex>
#include <stdexcept>
#include <string>

using namespace std;

namespace fs = std::filesystem;

fs::path getResultRootPath() {
    return "Result";
}

int findNextInputVersion() {
    fs::path resultRoot = getResultRootPath();
    regex versionPattern(R"(^DNA_SHORTREAD_ver([0-9]+)$)");
    int maxVersion = 0;

    if (!fs::exists(resultRoot)) {
        return 1;
    }

    for (const fs::directory_entry& entry :
         fs::directory_iterator(resultRoot)) {

        if (!entry.is_directory()) {
            continue;
        }

        smatch match;
        string folderName = entry.path().filename().string();

        if (regex_match(folderName, match, versionPattern)) {
            maxVersion = max(
                maxVersion,
                stoi(match[1].str())
            );
        }
    }

    return maxVersion + 1;
}

fs::path getInputVersionDir(int version) {
    if (version <= 0) {
        throw runtime_error("version은 1 이상의 정수여야 합니다.");
    }

    return getResultRootPath()
        / ("DNA_SHORTREAD_ver" + to_string(version));
}

fs::path getGenomeFilePath(int version) {
    return getInputVersionDir(version) / "DNA.txt";
}

fs::path getShortReadsDirPath(
    int version,
    int readLength
) {
    if (readLength <= 0) {
        throw runtime_error("readLength는 1 이상의 정수여야 합니다.");
    }

    return getInputVersionDir(version)
        / ("Shortreads_" + to_string(readLength));
}

fs::path getShortReadsFilePath(
    int version,
    int readLength,
    int runIndex
) {
    if (runIndex <= 0) {
        throw runtime_error("runIndex는 1 이상의 정수여야 합니다.");
    }

    return getShortReadsDirPath(version, readLength)
        / (
            "len" + to_string(readLength)
            + "_" + to_string(runIndex)
            + ".txt"
        );
}

fs::path getMappingResultJsonPath(
    int version,
    int readLength,
    int runIndex
) {
    if (readLength <= 0) {
        throw runtime_error("readLength는 1 이상의 정수여야 합니다.");
    }

    if (runIndex <= 0) {
        throw runtime_error("runIndex는 1 이상의 정수여야 합니다.");
    }

    return getShortReadsDirPath(version, readLength)
        / (
            "result_len" + to_string(readLength)
            + "_" + to_string(runIndex)
            + ".json"
        );
}