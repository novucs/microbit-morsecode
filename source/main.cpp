#include <string>
#include <MicroBit.h>
#include <queue>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

#define TICK_RATE 75

MicroBit uBit;
std::queue<bool> bits;

enum State {
    FIRST,
    PIPE
};

State state = FIRST;

void on_hi(MicroBitEvent event) {
    if (state == FIRST) {
        state = PIPE;
        return;
    }

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
            uBit.display.print(byte);
        }
        uBit.sleep(TICK_RATE);
    }
}

void write_bit(int bit) {
    uBit.io.P1.setDigitalValue(bit > 0);
    uBit.sleep(TICK_RATE);
}

void write_byte(char byte) {
    write_bit(byte & 0x80);
    write_bit(byte & 0x40);
    write_bit(byte & 0x20);
    write_bit(byte & 0x10);
    write_bit(byte & 0x08);
    write_bit(byte & 0x04);
    write_bit(byte & 0x02);
    write_bit(byte & 0x01);
}

void write_string(const std::string &message) {
    for (char character : message) {
        write_byte(character);
    }
}

void write() {
    uBit.display.print("a");
    while (uBit.io.P1.getDigitalValue() == 0) {}
    while (uBit.io.P1.getDigitalValue() == 1) {}

    uBit.sleep(500);
    uBit.display.print("b");
    uBit.io.P1.setDigitalValue(1);
    uBit.sleep(TICK_RATE);

    while (true) {
        write_string("hello friend. :) ");
    }
}

int main() {
    uBit.init();
    read();
//    write();
    return EXIT_SUCCESS;
}

#pragma clang diagnostic pop
