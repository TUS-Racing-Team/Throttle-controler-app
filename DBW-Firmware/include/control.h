#pragma once

void controlInit();
void controlTP();

// Fault state helpers, useful for debug/telemetry.
bool controlFaultLatched();

enum ControlFaultReason {
    CONTROL_FAULT_NONE = 0,
    CONTROL_FAULT_APPS,
    CONTROL_FAULT_TPS,
    CONTROL_FAULT_TRACKING
};

ControlFaultReason controlFaultReason();
const char* controlFaultReasonText();
bool controlClearFault();
