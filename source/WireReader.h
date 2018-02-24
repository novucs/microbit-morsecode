#ifndef MORSE_CODE_WIRE_READER_H
#define MORSE_CODE_WIRE_READER_H

#include <queue>
#include <MicroBit.h>
#include "MorseCode.h"

namespace morse_code {

    enum ReadState {
        FIRST,
        READ_LENGTH,
        READ_PAYLOAD
    };

    class WireReader {
    private:
        MicroBit *microBit;
        MicroBitPin *pin;
        uint16_t pinId;
        ReadState state = FIRST;
        std::queue<uint8_t> bytes;
        bool listening = false;
        uint8_t bits = 0;
        int bitsLength = 0;
        int packetLength = 0;
        int readLength = 0;
        int ignoreBits = 0;

        vector<uint8_t> readAll(int length);

        short readShort();

        void onLo(MicroBitEvent event);

        void onHi(MicroBitEvent event);

        void onByte(uint8_t byte);

    public:
        explicit WireReader(MicroBit *microBit);

        template<typename T>
        void listen(T *object, void (T::*handler)(std::vector<uint8_t>)) {
            pin->setDigitalValue(1);
            microBit->sleep(500);
            pin->setDigitalValue(0);

            pin->eventOn(MICROBIT_PIN_EVENT_ON_PULSE);
            microBit->messageBus.listen(pinId, MICROBIT_PIN_EVT_PULSE_HI, this, &WireReader::onHi);
            microBit->messageBus.listen(pinId, MICROBIT_PIN_EVT_PULSE_LO, this, &WireReader::onLo);
            listening = true;

            while (listening) {
                short packet_length = readShort();
                vector<uint8_t> payload = readAll(packet_length);
                (object->*handler)(payload);
                microBit->sleep(MORSE_CODE_TICK_RATE);
            }
        }
    };
}

#endif //MORSE_CODE_WIRE_READER_H
