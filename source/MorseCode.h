#ifndef MORSE_CODE_H
#define MORSE_CODE_H

#define MORSE_CODE_TICK_RATE 35

#include "WireReader.h"
#include "SpeckCipher.h"
#include "WireWriter.h"

namespace morse_code {

    static const int MAX_DOT_DURATION_MS = 250;
    static const int MAX_DASH_DURATION_MS = 750;
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

    class MorseCode {
    private:
        const MicroBitImage *dot = new MicroBitImage(5, 5, DOT_BITMAP);
        const MicroBitImage *dash = new MicroBitImage(5, 5, DASH_BITMAP);

        MicroBit *microBit = new MicroBit();
        SpeckCipher *cipher = new SpeckCipher();
        WireWriter *writer = new WireWriter(microBit);

        bool running = true;
        bool clicked = false;
        bool writing = false;

        Timer buttonTimer;
        std::string buffer{};
        std::vector<uint8_t> message{};

    public:
        uint8_t decode();

        void addBuffer();

        void onButtonAUp(MicroBitEvent);

        void onButtonADown(MicroBitEvent);

        void onButtonBClick(MicroBitEvent);

        void onPacket(vector <uint8_t> encrypted);

        void run();

        MicroBit *getMicroBit() const;
    };
}

#endif //MORSE_CODE_H
