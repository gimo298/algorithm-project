#include "GenerateInput.h"

#include "../DNA/GenerateDNA.h"
#include "../Reads/GenerateShortReads.h"

#include <filesystem>
#include <iostream>
#include <regex>
#include <string>

using namespace std;

namespace fs = std::filesystem;

int findNextInputVersion(const fs::path& resultRoot) {
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

fs::path generateInputFiles(const Config& cfg) {
    fs::path resultRoot = "Result";

    fs::create_directories(resultRoot);

    int nextVersion = findNextInputVersion(resultRoot);

    fs::path inputDir =
        resultRoot /
        ("DNA_SHORTREAD_ver" + to_string(nextVersion));

    fs::create_directories(inputDir);

    fs::path genomePath = inputDir / "DNA.txt";
    fs::path shortReadsPath = inputDir / "ShortReads.txt";

    generateDNAFile(cfg, genomePath);
    generateShortReadsFile(cfg, genomePath, shortReadsPath);

    cout << inputDir.string() << " 생성 완료\n";

    return inputDir;
}