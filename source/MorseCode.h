#ifndef MORSE_CODE_H
#define MORSE_CODE_H

// This is about as fast as the protocol can work without becoming unstable.
#define MORSE_CODE_TICK_RATE 35

#include "WireReader.h"
#include "SpeckCipher.h"
#include "WireWriter.h"

namespace morse_code {

    // The maximum durations in milliseconds to hold the button for the input
    // to be considered either a dash or a dot.
    static const int MAX_DOT_DURATION_MS = 250;
    static const int MAX_DASH_DURATION_MS = 750;

    // Bitmap images that display dots and dashes on the micro:bit LED matrix.
    // Displayed as the user is typing input or when sending a message.
    static const uint8_t DOT_BITMAP[] = {0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0,
                                         0, 0, 1, 0, 0,
                                         0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0};

    static const uint8_t DASH_BITMAP[] = {0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0,
                                          0, 1, 1, 1, 0,
                                          0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0};

    /**
     * The main morse code manager. Holds and manages the current program state.
     * All morse code may be input by the user by tapping the A button. The
     * entire message is cached locally until button B is pressed. Messages are
     * encoded in ASCII, encrypted and sent as the contents of a packet.
     */
    class MorseCode {
    private:
        // Dot and dash images to be pasted to the LED matrix, built using the
        // bitmaps defined above.
        const MicroBitImage *dot = new MicroBitImage(5, 5, DOT_BITMAP);
        const MicroBitImage *dash = new MicroBitImage(5, 5, DASH_BITMAP);

        // The micro:bit instance, used for interacting with the hardware.
        MicroBit *microBit = new MicroBit();

        // Speck cipher reference, state must be retained due to the usage of
        // the initialization vector for cipher block chaining.
        SpeckCipher *cipher = new SpeckCipher();

        // The wire writer, provides a simple interface for writing packets
        // via GPIO.
        WireWriter *writer = new WireWriter(microBit);

        // Always set to true, mostly here to stop my IDE from annoying me
        // about endless loops. Set to false to exit the program.
        bool running = true;

        // Indicates when the user is still providing input for a character.
        bool clicked = false;

        // Indicates when the program is sending a message over the wire. Used
        // for quickly animating dashes and dots to let the user know we're
        // busy doing something.
        bool writing = false;

        // Timer for how long a button was held down, required for event based
        // implementation of morse code user input.
        Timer buttonTimer;

        // Morse code buffer, only contains dashes and dots as the user enters
        // their input.
        std::string buffer{};

        // Translated ASCII message. Appended to each time the user
        // successfully enters a valid sequence of morse code.
        std::vector<uint8_t> message{};

        /**
         * Translates the current buffer to an ASCII character.
         *
         * @return the encoded character, or {@code ' '} if not found.
         */
        uint8_t translate();

        /**
         * Attempts to translate and add the morse code buffer to the message.
         */
        void addBuffer();

        /**
         * Handles when button A is released. Used for inputting morse code.
         */
        void onButtonAUp(MicroBitEvent);

        /**
         * Handles when button A is pressed. Used for inputting morse code.
         */
        void onButtonADown(MicroBitEvent);

        /**
         * Handles when button B is clicked. Used for sending the message.
         */
        void onButtonBClick(MicroBitEvent);

    public:

        /**
         * Handles when a packet is received over GPIO.
         *
         * @param encrypted the encrypted message.
         */
        void onPacket(std::vector<uint8_t> encrypted);

        /**
         * Initialises and runs the program.
         */
        void run();

        /**
         * Gets the micro:bit instance.
         *
         * @return the micro:bit instance.
         */
        MicroBit *getMicroBit() const;
    };
}

#endif //MORSE_CODE_H
