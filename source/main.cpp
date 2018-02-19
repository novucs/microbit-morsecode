#include <string>
#include <vector>
#include <unordered_map>
#include <MicroBit.h>

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

void read() {
    // Begin reading.
    std::vector<char> buffer;
    std::vector<char> message;
    long waiting = 0;
    long started_to_wait = uBit.systemTime();
    long key_down_time = 0;
    long key_up_time = 0;
    long duration = 0;
    char character;

    while (true) {
        waiting = uBit.systemTime() - started_to_wait;
        key_down_time = 0;

        while (uBit.buttonA.isPressed()) {
            uBit.io.P1.setDigitalValue(1);

            if (key_down_time == 0) {
                key_down_time = uBit.systemTime();
            }
        }

        while (uBit.io.P2.getDigitalValue() == 1) {
            if (key_down_time == 0) {
                key_down_time = uBit.systemTime();
            }
        }

        key_up_time = uBit.systemTime();
        uBit.io.P1.setDigitalValue(0);

        if (key_down_time != 0) {
            duration = key_up_time - key_down_time;

            if (duration < dot_threshold) {
                buffer.push_back('.');
                uBit.display.image.paste(dot);
            } else if (duration < dash_threshold) {
                buffer.push_back('-');
                uBit.display.image.paste(dash);
            }

            started_to_wait = uBit.systemTime();
        } else if (buffer.size() > 0 && waiting > dash_threshold) {
            character = decode(buffer);
            buffer.clear();

            if (character != ' ') {
                uBit.display.print(character);
                message.push_back(character);
            }
        }

        if (uBit.buttonB.isPressed()) {
            ManagedString msg(message.data(), static_cast<const int16_t>(message.size()));
            uBit.display.scroll(msg);
            message.clear();
        }
    }
}

void write() {
    // Begin writing.
    while (true) {
        uBit.io.P1.setDigitalValue(1);
        uBit.sleep(150);
        uBit.io.P1.setDigitalValue(0);
        uBit.sleep(50);
        uBit.io.P1.setDigitalValue(1);
        uBit.sleep(400);
        uBit.io.P1.setDigitalValue(0);
        uBit.sleep(50);
        uBit.io.P1.setDigitalValue(1);
        uBit.sleep(400);
        uBit.io.P1.setDigitalValue(0);

        uBit.sleep(3000);
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