#pragma once

#include "../Config.h"

#include <filesystem>

void runMappingExperiment(
    const Config& baseConfig,
    int inputVersion,
    int readLength,
    int runIndex,
    const std::filesystem::path& genomePath,
    const std::filesystem::path& shortReadsPath,
    const std::filesystem::path& outputPath
);