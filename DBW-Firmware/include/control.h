#pragma once

void controlInit();
void controlTP();

// Fault state helpers, useful for debug/telemetry.
bool controlFaultLatched();
void controlClearFault();
