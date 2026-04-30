#pragma once

#include "read_data.h"

// Read throttle position in percent.
// Returns valid=false if raw range or correlation check fails.
ReadData readThrottlePct();
