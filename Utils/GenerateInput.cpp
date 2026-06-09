#include "GenerateInput.h"

#include "../DNA/GenerateDNA.h"
#include "../Reads/GenerateShortReads.h"
#include "IO.h"
#include "ResultPaths.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

namespace fs = std::filesystem;

void ensureInputVersionDirectory(int inputVersion) {
    fs::path resultRoot = getResultRootPath();

    fs::create_directories(resultRoot);
    fs::path inputDir = getInputVersionDir(inputVersion);

    if (fs::exists(inputDir)) {
        cout << inputDir.string() << " 폴더가 이미 존재하여 생략합니다.\n";
        return;
    }

    fs::create_directories(inputDir);
    cout << inputDir.string() << " 폴더 생성 완료\n";
}

void ensureGenomeFile(
    const Config& cfg,
    int inputVersion
) {
    fs::path genomePath = getGenomeFilePath(inputVersion);

    if (fs::exists(genomePath)) {
        string genome = loadGenome(genomePath);

        if (static_cast<int>(genome.size()) != cfg.length) {
            throw runtime_error(
                genomePath.string()
                + " 파일 길이가 main에서 선택한 DNA 길이와 일치하지 않습니다."
            );
        }

        cout << genomePath.string() << " 파일이 이미 존재하여 생략합니다.\n";
        return;
    }

    generateDNAFile(cfg, genomePath);
}

void ensureShortReadsDirectory(
    int inputVersion,
    int readLength
) {
    fs::path shortReadsDir =
        getShortReadsDirPath(
            inputVersion,
            readLength
        );

    if (fs::exists(shortReadsDir)) {
        cout << shortReadsDir.string() << " 폴더가 이미 존재하여 생략합니다.\n";
        return;
    }

    fs::create_directories(shortReadsDir);
    cout << shortReadsDir.string() << " 폴더 생성 완료\n";
}

std::filesystem::path ensureShortReadsFile(
    const Config& baseConfig,
    int inputVersion,
    int readLength,
    int runIndex
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

    fs::path genomePath = getGenomeFilePath(inputVersion);

    if (!fs::exists(genomePath)) {
        throw runtime_error(genomePath.string() + " 파일이 존재하지 않습니다.");
    }

    fs::path shortReadsPath =
        getShortReadsFilePath(
            inputVersion,
            readLength,
            runIndex
        );

    if (fs::exists(shortReadsPath)) {
        vector<string> shortReads = loadShortReads(shortReadsPath);

        if (static_cast<int>(shortReads.size()) != baseConfig.CntOfReads) {
            throw runtime_error(
                shortReadsPath.string()
                + " 파일의 read 개수가 현재 설정과 일치하지 않습니다."
            );
        }

        for (const string& shortRead : shortReads) {
            if (static_cast<int>(shortRead.size()) != readLength) {
                throw runtime_error(
                    shortReadsPath.string()
                    + " 파일의 read 길이가 현재 설정과 일치하지 않습니다."
                );
            }
        }

        cout << shortReadsPath.string() << " 파일이 이미 존재하여 생략합니다.\n";
        return shortReadsPath;
    }

    Config cfg = baseConfig;
    cfg.LenOfReads = readLength;

    generateShortReadsFile(
        cfg,
        genomePath,
        shortReadsPath
    );

    return shortReadsPath;
}