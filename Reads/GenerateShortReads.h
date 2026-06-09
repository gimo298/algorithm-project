#pragma once

#include "../Config.h"

#include <filesystem>

void generateShortReadsFile(const Config& cfg);

void generateShortReadsFile(
	const Config& cfg,
	const std::filesystem::path& genomePath,
	const std::filesystem::path& outputPath
);
