#include <string>
#include <vector>
#include <unordered_map>
#include <MicroBit.h>
#include <wait_api.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

MicroBit uBit;

uint8_t dot_image[] = {0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0,
                       0, 0, 1, 0, 0,
                       0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0};

uint8_t dash_image[] = {0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 1, 1, 1, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0};

MicroBitImage dot(5, 5, dot_image);
MicroBitImage dash(5, 5, dash_image);

std::unordered_map<std::string, char> morse{
        {".-",    'A'},
        {"-...",  'B'},
        {"-.-.",  'C'},
        {"-..",   'D'},
        {".",     'E'},
        {"..-.",  'F'},
        {"--.",   'G'},
        {"....",  'H'},
        {"..",    'I'},
        {".---",  'J'},
        {"-.-",   'K'},
        {".-..",  'L'},
        {"--",    'M'},
        {"-.",    'N'},
        {"---",   'O'},
        {".--.",  'P'},
        {"--.-",  'Q'},
        {".-.",   'R'},
        {"...",   'S'},
        {"-",     'T'},
        {"..-",   'U'},
        {"...-",  'V'},
        {".--",   'W'},
        {"-..-",  'X'},
        {"-.--",  'Y'},
        {"--..",  'Z'},
        {".----", '1'},
        {"..---", '2'},
        {"...--", '3'},
        {"....-", '4'},
        {".....", '5'},
        {"-....", '6'},
        {"--...", '7'},
        {"---..", '8'},
        {"----.", '9'},
        {"-----", '0'}
};

static const int dot_threshold = 250;
static const int dash_threshold = 500;

char decode(std::vector<char> &buffer) {
    std::string code(buffer.begin(), buffer.end());
    auto iterator = morse.find(code);
    if (iterator == morse.end()) {
        return ' ';
    }
    return iterator->second;
}

//void read() {
//    // Begin reading.
//    std::vector<char> buffer;
//    std::vector<char> message;
//    long waiting = 0;
//    long started_to_wait = uBit.systemTime();
//    long key_down_time = 0;
//    long key_up_time = 0;
//    long duration = 0;
//    char character;
//
//    while (true) {
//        waiting = uBit.systemTime() - started_to_wait;
//        key_down_time = 0;
//
//        while (uBit.buttonA.isPressed()) {
//            uBit.io.P1.setDigitalValue(1);
//
//            if (key_down_time == 0) {
//                key_down_time = uBit.systemTime();
//            }
//        }
//
//        while (uBit.io.P2.getDigitalValue() == 1) {
//            if (key_down_time == 0) {
//                key_down_time = uBit.systemTime();
//            }
//        }
//
//        key_up_time = uBit.systemTime();
//        uBit.io.P1.setDigitalValue(0);
//
//        if (key_down_time != 0) {
//            duration = key_up_time - key_down_time;
//
//            if (duration < dot_threshold) {
//                buffer.push_back('.');
//                uBit.display.image.paste(dot);
//            } else if (duration < dash_threshold) {
//                buffer.push_back('-');
//                uBit.display.image.paste(dash);
//            }
//
//            started_to_wait = uBit.systemTime();
//        } else if (buffer.size() > 0 && waiting > dash_threshold) {
//            character = decode(buffer);
//            buffer.clear();
//
//            if (character != ' ') {
//                uBit.display.print(character);
//                message.push_back(character);
//            }
//        }
//
//        if (uBit.buttonB.isPressed()) {
//            ManagedString msg(message.data(), static_cast<const int16_t>(message.size()));
//            uBit.display.scroll(msg);
//            message.clear();
//        }
//        uBit.sleep(1);
//    }

char input = 0;

#define TICK_RATE 25

void onHi(MicroBitEvent event) {
    uint64_t duration = (event.timestamp / 1000) / TICK_RATE;

    for (int i = 0; i < duration; i++) {
        input <<= 1;
        input += 1;
    }

    if (input == 0x1B) {
        uBit.display.print("A");
    }
}

void onLo(MicroBitEvent event) {
    uint64_t duration = (event.timestamp / 1000) / TICK_RATE;
    input <<= duration;
    if (input == 0x1A) {
        uBit.display.print("B");
    }
}

void read() {
    uBit.io.P2.eventOn(MICROBIT_PIN_EVENT_ON_PULSE);
    uBit.messageBus.listen(MICROBIT_ID_IO_P2, MICROBIT_PIN_EVT_PULSE_HI, onHi, MESSAGE_BUS_LISTENER_IMMEDIATE);
    uBit.messageBus.listen(MICROBIT_ID_IO_P2, MICROBIT_PIN_EVT_PULSE_LO, onLo, MESSAGE_BUS_LISTENER_IMMEDIATE);

    while (true) {
        uBit.sleep(500);
    }
}

void write(std::vector<int> bits) {
    for (int bit : bits) {
        uBit.io.P1.setDigitalValue(bit);
        uBit.sleep(TICK_RATE);
    }
}

void writeBit(int bit) {
    if (bit) {
        uBit.io.P1.setDigitalValue(1);
    } else {
        uBit.io.P1.setDigitalValue(0);
    }
    uBit.sleep(TICK_RATE);
}

void write(int byte) {
    writeBit(byte & 0x80);
    writeBit(byte & 0x40);
    writeBit(byte & 0x20);
    writeBit(byte & 0x10);
    writeBit(byte & 0x08);
    writeBit(byte & 0x04);
    writeBit(byte & 0x02);
    writeBit(byte & 0x01);
    if (!(byte & 0x01)) {
        uBit.io.P1.setDigitalValue(1);
        uBit.sleep(TICK_RATE);
        uBit.io.P1.setDigitalValue(0);
        uBit.sleep(TICK_RATE);
    }
}

void write() {
    // Begin writing.
    while (true) {
        // 0001 1011
//        write({0, 0, 0, 1, 1, 0, 1, 1});
        write(0x1B);
        write(0x1A);
    }
}

int main() {
    uBit.init();

    // Read on a different fiber.
    read();
//    write();

    return EXIT_SUCCESS;
}

#pragma clang diagnostic pop
