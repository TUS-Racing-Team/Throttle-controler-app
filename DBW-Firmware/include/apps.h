#pragma once

#include "read_data.h"

// Read accelerator pedal position in percent.
// Returns valid=false if raw range or correlation check fails.
ReadData readAppsPct();
