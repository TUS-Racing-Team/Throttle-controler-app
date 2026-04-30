#include "canbus.h"

#include <Arduino.h>
#include <due_can.h>

static CanFrame queue[64];
static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;

static inline bool push(const CanFrame& f) {
    uint8_t n = (head + 1) & 63;

    if (n == tail) {
        return false;
    }

    queue[head] = f;
    head = n;

    return true;
}

bool canbusPop(CanFrame& out) {
    if (tail == head) {
        return false;
    }

    out = queue[tail];
    tail = (tail + 1) & 63;

    return true;
}

void canbusInit(uint32_t bitrate) {
    Can0.begin(bitrate);
    Can0.watchFor();
    Can0.setNumTXBoxes(1);
}

void canbusUpdate() {
    CAN_FRAME f;

    int maxFrames = 8;

    while (maxFrames-- > 0 && Can0.available()) {
        Can0.read(f);

        CanFrame out{};
        out.id = f.id;
        out.extended = f.extended;
        out.len = f.length;

        if (out.len > 8) {
            out.len = 8;
        }

        for (int i = 0; i < out.len; i++) {
            out.data[i] = f.data.bytes[i];
        }

        push(out);
    }
}
