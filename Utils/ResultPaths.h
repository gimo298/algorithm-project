#pragma once

#include <filesystem>

int findNextInputVersion();

std::filesystem::path getResultRootPath();

std::filesystem::path getInputVersionDir(int version);

std::filesystem::path getGenomeFilePath(int version);

std::filesystem::path getShortReadsDirPath(
    int version,
    int readLength
);

std::filesystem::path getShortReadsFilePath(
    int version,
    int readLength,
    int runIndex
);

std::filesystem::path getMappingResultJsonPath(
    int version,
    int readLength,
    int runIndex
);