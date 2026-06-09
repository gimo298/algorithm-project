#pragma once

#include "../Config.h"

#include <filesystem>

void ensureInputVersionDirectory(int inputVersion);

void ensureGenomeFile(
	const Config& cfg,
	int inputVersion
);

void ensureShortReadsDirectory(
	int inputVersion,
	int readLength
);

std::filesystem::path ensureShortReadsFile(
	const Config& baseConfig,
	int inputVersion,
	int readLength,
	int runIndex
);