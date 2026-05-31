#pragma once

#include "../Config.h"

#include <filesystem>

void generateDNAFile(const Config& cfg);

void generateDNAFile(
	const Config& cfg,
	const std::filesystem::path& outputPath
);
