#include <string>
#include <MicroBit.h>
#include <unordered_map>
#include "WireReader.h"
#include "WireWriter.h"
#include "SpeckCipher.h"

MicroBit microBit;
SpeckCipher cipher;
bool running = true;
auto writer = new WireWriter(&microBit, &microBit.io.P1);
auto reader = new WireReader(&microBit, &microBit.io.P2);

MicroBitImage dot(5, 5, (uint8_t[25]) {0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0,
                                       0, 0, 1, 0, 0,
                                       0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0});

MicroBitImage dash(5, 5, (uint8_t[25]) {0, 0, 0, 0, 0,
                                        0, 0, 0, 0, 0,
                                        0, 1, 1, 1, 0,
                                        0, 0, 0, 0, 0,
                                        0, 0, 0, 0, 0});

std::unordered_map<std::string, uint8_t> morse{
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

Timer buttonTimer;
int maxDotDurationMs = 250;
int maxDashDurationMs = 750;
bool clicked = false;
bool writing = false;
std::string buffer;
std::vector<uint8_t> message;

uint8_t decode() {
    auto iterator = morse.find(buffer);
    if (iterator == morse.end()) {
        return ' ';
    }
    return iterator->second;
}

void addBuffer() {
    if (buttonTimer.read_ms() <= maxDashDurationMs || !clicked) return;

    uint8_t character = decode();
    buffer.clear();

    if (character != ' ') {
        microBit.display.print((char) character);
        microBit.sleep(500);
        message.push_back(character);
    }

    clicked = false;
}

void onButtonAUp(MicroBitEvent) {
    microBit.display.stopAnimation();
    if (buttonTimer.read_ms() < maxDotDurationMs) {
        buffer += '.';
        microBit.display.image.paste(dot);
    } else {
        buffer += '-';
        microBit.display.image.paste(dash);
    }
}

void onButtonADown(MicroBitEvent) {
    buttonTimer.reset();
    clicked = true;
}

void onButtonBClick(MicroBitEvent) {
    addBuffer();
    writing = true;
    writer->write(cipher.encrypt(message));
    writing = false;
    ManagedString msg((char *) message.data(), (uint16_t) message.size());
    microBit.display.scroll(msg);
    message.clear();
}

void onPacket(std::vector<uint8_t> encrypted) {
    std::vector<uint8_t> packet = cipher.decrypt(encrypted);
    ManagedString message((char *) packet.data(), (uint16_t) packet.size());
    microBit.display.scroll(message);
}

void read() {
    reader->listen(onPacket);
}

int main() {
    microBit.init();
    create_fiber(read);
    microBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_DOWN, onButtonADown);
    microBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_UP, onButtonAUp);
    microBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onButtonBClick);
    buttonTimer.start();

    bool printDash = false;

    while (running) {
        addBuffer();

        if (writing) {
            microBit.display.image.paste(printDash ? dash : dot);
            printDash = !printDash;
        }

        microBit.sleep(100);
    }

    return EXIT_SUCCESS;
}
