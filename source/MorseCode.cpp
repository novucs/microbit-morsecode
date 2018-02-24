#include <string>
#include <unordered_map>
#include <MicroBitImage.h>
#include <MicroBitEvent.h>
#include "MorseCode.h"

namespace morse_code {
    namespace {
        const std::unordered_map<std::string, uint8_t> MORSE_TO_CHARACTER{
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

        void read(MorseCode *morseCode) {
            auto reader = new WireReader(morseCode->getMicroBit());
            reader->listen(morseCode, &MorseCode::onPacket);
        }
    }

    uint8_t MorseCode::translate() {
        auto iterator = MORSE_TO_CHARACTER.find(buffer);
        if (iterator == MORSE_TO_CHARACTER.end()) {
            return ' ';
        }
        return iterator->second;
    }

    void MorseCode::addBuffer() {
        if (buttonTimer.read_ms() <= MAX_DASH_DURATION_MS || !clicked) return;

        uint8_t character = translate();
        buffer.clear();

        if (character != ' ') {
            microBit->display.print((char) character);
            microBit->sleep(500);
            message.push_back(character);
        }

        clicked = false;
    }

    void MorseCode::onButtonAUp(MicroBitEvent) {
        microBit->display.stopAnimation();
        if (buttonTimer.read_ms() < MAX_DOT_DURATION_MS) {
            buffer += '.';
            microBit->display.image.paste(*dot);
        } else {
            buffer += '-';
            microBit->display.image.paste(*dash);
        }
    }

    void MorseCode::onButtonADown(MicroBitEvent) {
        buttonTimer.reset();
        clicked = true;
    }

    void MorseCode::onButtonBClick(MicroBitEvent) {
        // Add the last input character to the buffer, if possible.
        addBuffer();

        // Encrypt and send the message.
        writing = true;
        writer->write(cipher->encrypt(message));
        writing = false;

        // Print the message to screen.
        ManagedString msg((char *) message.data(), (uint16_t) message.size());
        microBit->display.scroll(msg);

        // Clear the message.
        message.clear();
    }

    void MorseCode::onPacket(std::vector<uint8_t> encrypted) {
        // Decrypt the message.
        std::vector<uint8_t> packet = cipher->decrypt(encrypted);

        // Print the message to screen.
        ManagedString message((char *) packet.data(), (uint16_t) packet.size());
        microBit->display.scroll(message);
    }

    void MorseCode::run() {
        microBit->init();

        create_fiber((void (*)(void *)) read, (void *) this);
        microBit->messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_DOWN, this, &MorseCode::onButtonADown);
        microBit->messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_UP, this, &MorseCode::onButtonAUp);
        microBit->messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, this, &MorseCode::onButtonBClick);
        buttonTimer.start();

        microBit->display.scroll("MORSE CODE");
        bool printDash = false;

        while (running) {
            addBuffer();

            if (writing) {
                microBit->display.image.paste(printDash ? *dash : *dot);
                printDash = !printDash;
            }

            microBit->sleep(100);
        }
    }

    MicroBit *MorseCode::getMicroBit() const {
        return microBit;
    }
}
