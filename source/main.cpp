#include <string>
#include <MicroBit.h>
#include <queue>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

#define TICK_RATE 75

enum State {
    FIRST,
    PIPE
};

State state = FIRST;
MicroBit uBit;
std::queue<char> bytes;
char bits = 0;
int bits_size = 0;

void on_hi(MicroBitEvent event) {
    if (state == FIRST) {
        state = PIPE;
        return;
    }

    uint64_t ticks = (event.timestamp / 1000) / TICK_RATE;

    for (int i = 0; i < ticks; i++) {
        bits <<= 1;
        bits += 1;
        bits_size += 1;

        if (bits_size == 8) {
            bytes.push(bits);
            bits = 0;
            bits_size = 0;
        }
    }
}

void on_lo(MicroBitEvent event) {
    uint64_t ticks = (event.timestamp / 1000) / TICK_RATE;

    for (int i = 0; i < ticks; i++) {
        bits <<= 1;
        bits_size += 1;

        if (bits_size == 8) {
            bytes.push(bits);
            bits = 0;
            bits_size = 0;
        }
    }
}

std::vector<char> read_all(int length) {
    vector<char> target;

    while (true) {
        while (bytes.size() > 0 && length > 0) {
            const char byte = bytes.front();
            bytes.pop();
            target.push_back(byte);
            length -= 1;
        }

        if (length <= 0) {
            return target;
        }

        uBit.sleep(TICK_RATE);
    }
}

short read_short() {
    auto bytes = read_all(2);
    return (bytes[0] << 4) + bytes[1];
}

void read() {
    uBit.io.P2.setDigitalValue(1);
    uBit.sleep(500);
    uBit.io.P1.setDigitalValue(0);

    uBit.io.P2.eventOn(MICROBIT_PIN_EVENT_ON_PULSE);
    uBit.messageBus.listen(MICROBIT_ID_IO_P2, MICROBIT_PIN_EVT_PULSE_HI, on_hi);
    uBit.messageBus.listen(MICROBIT_ID_IO_P2, MICROBIT_PIN_EVT_PULSE_LO, on_lo);

    while (true) {
        short packet_length = read_short();
        std::vector<char> payload = read_all(packet_length);
        uBit.display.scroll(ManagedString(payload.data(), (uint16_t)payload.size()));
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

void write_short(short value) {
    write_byte((char)(value >> 8));
    write_byte((char)(value & 0xFF));
}

void write_string(const std::string &message) {
    write_short((short)message.size());
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
        write_string("hello friend. :)");
    }
}

int main() {
    uBit.init();
//    read();
    write();
    return EXIT_SUCCESS;
}

#pragma clang diagnostic pop
