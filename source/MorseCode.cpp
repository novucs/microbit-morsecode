#include <string>
#include <unordered_map>
#include <MicroBitImage.h>
#include <MicroBitEvent.h>
#include "MorseCode.h"

namespace morse_code {
    namespace {
        // Morse code to ASCII translation map.
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

        /**
         * Registers the {@link MorseCode#onPacket} listener.
         *
         * @param morseCode the {@link MorseCode} program instance.
         */
        void read(MorseCode *morseCode) {
            // Create a new reader and start listening to it.
            auto reader = new WireReader(morseCode->getMicroBit());
            reader->listen(morseCode, &MorseCode::onPacket);
        }
    }

    uint8_t MorseCode::translate() {
        // Find and return the character translation for the provided morse
        // code buffer. If no translation found, return a space character.
        auto iterator = MORSE_TO_CHARACTER.find(buffer);
        if (iterator == MORSE_TO_CHARACTER.end()) {
            return ' ';
        }
        return iterator->second;
    }

    void MorseCode::addBuffer() {
        // Do nothing if more input may come in, or if no input was provided.
        if (buttonTimer.read_ms() <= MAX_DASH_DURATION_MS || !clicked) {
            return;
        }

        // Translate then clear the morse code buffer.
        uint8_t character = translate();
        buffer.clear();

        // Display character on the LED screen and add to the message.
        if (character != ' ') {
            microBit->display.print((char) character);
            message.push_back(character);
        }

        // All input has been consumed.
        clicked = false;
    }

    void MorseCode::sendPacket(const std::vector<uint8_t> &plainTextPacket) {
        // Encrypt and send the message.
        writing = true;
        writer->write(cipher->encrypt(plainTextPacket));
        writing = false;
    }

    void MorseCode::onButtonAUp(MicroBitEvent) {
        // Do nothing if writing or button was not clicked (prevents concurrent
        // buffer modification).
        if (writing || !clicked) {
            return;
        }

        // Stop any animation currently happening.
        microBit->display.stopAnimation();

        // Update buffer depending on intended input and display to the user
        // whether their input was registered as a dash or a dot.
        if (buttonTimer.read_ms() < MAX_DOT_DURATION_MS) {
            buffer += '.';
            microBit->display.image.paste(*dot);
        } else {
            buffer += '-';
            microBit->display.image.paste(*dash);
        }
    }

    void MorseCode::onButtonADown(MicroBitEvent) {
        // Do nothing if writing.
        if (writing) {
            return;
        }

        // Reset button timer and update state to button clicked.
        buttonTimer.reset();
        clicked = true;
    }

    void MorseCode::onButtonBClick(MicroBitEvent) {
        // Add the last input character to the buffer, if possible.
        addBuffer();

        // Do nothing if there is no message to send.
        if (message.size() <= 0) {
            return;
        }

        // Copy and clear the message.
        std::vector<uint8_t> message;
        message.push_back(MESSAGE);
        message.insert(message.end(), MorseCode::message.begin(), MorseCode::message.end());
        MorseCode::message.clear();
        sendPacket(message);

        // Print the message to screen.
        ManagedString text((char *) (message.data() + 1), (uint16_t) (message.size() - 1));
        microBit->display.scroll(text);
    }

    void MorseCode::onButtonABClick(MicroBitEvent) {
        // Clear the current message, to avoid duplicate input.
        MorseCode::message.clear();

        // Send a happy packet.
        std::vector<uint8_t> message;
        message.push_back(HAPPY);
        sendPacket(message);

        // Stop all current animation and display a happy face.
        microBit->display.stopAnimation();
        microBit->display.image.paste(*happy);
        microBit->sleep(500);
    }

    void MorseCode::onShake(MicroBitEvent) {
        // Clear the current message, to avoid duplicate input.
        MorseCode::message.clear();

        // Send a sad packet.
        std::vector<uint8_t> message;
        message.push_back(SAD);
        sendPacket(message);

        // Stop all current animation and display a sad face.
        microBit->display.stopAnimation();
        microBit->display.image.paste(*sad);
        microBit->sleep(500);
    }

    void MorseCode::onPacket(std::vector<uint8_t> encryptedPacket) {
        // Decrypt the message.
        std::vector<uint8_t> packet = cipher->decrypt(encryptedPacket);

        // Get the packet type.
        PacketType packetType = (PacketType) packet.at(0);

        // Stop all current animation.
        microBit->display.stopAnimation();

        switch (packetType) {
            case MESSAGE: {
                // Print the message to screen.
                ManagedString message((char *) packet.data() + 1, (uint16_t) (packet.size() - 1));
                microBit->display.scroll(message);
                break;
            }
            case HAPPY: {
                // Print a happy face.
                microBit->display.image.paste(*happy);
                microBit->sleep(500);
                break;
            }
            case SAD: {
                // Print a sad face.
                microBit->display.image.paste(*sad);
                microBit->sleep(500);
                break;
            }
        }
    }

    void MorseCode::run() {
        // Initialise the micro:bit.
        microBit->init();

        // Setup packet and button event listeners.
        create_fiber((void (*)(void *)) read, (void *) this);
        microBit->messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_DOWN, this, &MorseCode::onButtonADown);
        microBit->messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_UP, this, &MorseCode::onButtonAUp);
        microBit->messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, this, &MorseCode::onButtonBClick);
        microBit->messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_CLICK, this, &MorseCode::onButtonABClick);
        microBit->messageBus.listen(MICROBIT_ID_GESTURE, MICROBIT_ACCELEROMETER_EVT_SHAKE, this, &MorseCode::onShake);

        // Start the button timer.
        buttonTimer.start();

        // Display welcome message.
        microBit->display.scroll("MORSE CODE");

        // Used for determining whether to print a dash or dot when displaying
        // the message sending animation.
        bool printDash = false;

        // Main program loop.
        while (running) {
            // Attempt to add current buffered input to the message.
            addBuffer();

            // Display animation when sending a packet.
            if (writing) {
                microBit->display.image.paste(printDash ? *dash : *dot);
                printDash = !printDash;
            }

            // Tick once every 100 milliseconds.
            microBit->sleep(100);
        }
    }

    MicroBit *MorseCode::getMicroBit() const {
        return microBit;
    }
}
