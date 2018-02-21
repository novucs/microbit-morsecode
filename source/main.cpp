#include <string>
#include <MicroBit.h>
#include <queue>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

#define TICK_RATE 500

MicroBit uBit;
std::queue<bool> bits;

void on_hi(MicroBitEvent event) {
    uint64_t ticks = (event.timestamp / 1000) / TICK_RATE;
    for (int i = 0; i < ticks; i++) {
        bits.push(true);
    }
}

void on_lo(MicroBitEvent event) {
    uint64_t ticks = (event.timestamp / 1000) / TICK_RATE;
    for (int i = 0; i < ticks; i++) {
        bits.push(false);
    }
}

void read() {
    uBit.io.P2.setDigitalValue(1);
    uBit.sleep(500);
    uBit.io.P1.setDigitalValue(0);

    uBit.io.P2.eventOn(MICROBIT_PIN_EVENT_ON_PULSE);
    uBit.messageBus.listen(MICROBIT_ID_IO_P2, MICROBIT_PIN_EVT_PULSE_HI, on_hi);
    uBit.messageBus.listen(MICROBIT_ID_IO_P2, MICROBIT_PIN_EVT_PULSE_LO, on_lo);

    while (true) {
        while (bits.size() > 8) {
            char byte = 0;
            for (int i = 0; i < 8; i++) {
                const bool bit = bits.front();
                bits.pop();
                byte <<= 1;
                byte += bit;
            }
            byte = static_cast<char>(((byte * 0x80200802ULL) & 0x0884422110ULL) * 0x0101010101ULL >> 32);
            uBit.display.scroll((int)byte);
            uBit.sleep(500);
        }
        uBit.sleep(500);
    }
}

void write_bit(int bit) {
    if (bit) {
        uBit.io.P1.setDigitalValue(1);
    } else {
        uBit.io.P1.setDigitalValue(0);
    }
    uBit.sleep(TICK_RATE);
}

void write_byte(int byte) {
    write_bit(byte & 0x80);
    write_bit(byte & 0x40);
    write_bit(byte & 0x20);
    write_bit(byte & 0x10);
    write_bit(byte & 0x08);
    write_bit(byte & 0x04);
    write_bit(byte & 0x02);
    write_bit(byte & 0x01);
}

void write() {
    uBit.display.print("a");
    while (uBit.io.P1.getDigitalValue() == 0) {}
    while (uBit.io.P1.getDigitalValue() == 1) {}

    uBit.sleep(500);
    uBit.display.print("b");

    while (true) {
        write_byte(27);
    }
}

int main() {
    uBit.init();
//    read();
    write();
    return EXIT_SUCCESS;
}

#pragma clang diagnostic pop
