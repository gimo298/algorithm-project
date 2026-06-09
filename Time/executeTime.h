#ifndef EXECUTETIME_H
#define EXECUTETIME_H

#include "../Config.h"

#include <string>
#include <vector>

void measureAndSaveExecutionTimes(
	const Config& cfg,
	const std::string& genome,
	const std::vector<std::string>& shortReads
);

#endif // EXECUTETIME_H
