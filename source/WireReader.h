#ifndef MORSE_CODE_WIRE_READER_H
#define MORSE_CODE_WIRE_READER_H

#include <queue>
#include <MicroBit.h>
#include "MorseCode.h"

namespace morse_code {

    /**
     * Read state, used in the {@link WireReader} pipe.
     */
    enum ReadState {
        // Consumes the first connection bit.
        FIRST,

        // Reads the packet length.
        READ_LENGTH,

        // Reads the packet payload.
        READ_PAYLOAD
    };

    /**
     * Utility for reading packets over GPIO on the micro:bit.
     */
    class WireReader {
    private:
        // The micro:bit instance.
        MicroBit *microBit;

        // The pin and its ID to read packets from.
        MicroBitPin *pin;
        uint16_t pinId;

        // Current state of reading.
        ReadState state = FIRST;

        // All unconsumed bytes read from GPIO.
        std::queue<uint8_t> bytes;

        // Indicates whether we are currently reading from GPIO.
        bool listening = false;

        // Latest bits read in.
        uint8_t bits = 0;
        int bitsLength = 0;

        // Length of the packet currently being read.
        int packetLength = 0;

        // Length read from the GPIO. Used in conjunction with packet length to
        // determine when each packet stops and starts.
        int readLength = 0;

        // Number of bits to ignore, used for consuming packet barriers.
        int ignoreBits = 0;

        /**
         * Reads all bytes into a buffer over a specified length.
         *
         * @param length the number of bytes to read in.
         * @return the buffered bytes.
         */
        std::vector<uint8_t> readAll(int length);

        /**
         * Reads a short (2 bytes) from the wire.
         *
         * @return the next read in short.
         */
        uint16_t readShort();

        /**
         * Handles when the GPIO signal is set to LO.
         *
         * @param event the LO event.
         */
        void onLo(MicroBitEvent event);

        /**
         * Handles when the GPIO signal is set to HI.
         *
         * @param event the HI event.
         */
        void onHi(MicroBitEvent event);

        /**
         * Handles when a full byte is read from GPIO.
         *
         * @param byte the newly read in byte.
         */
        void onByte(uint8_t byte);

    public:

        /**
         * Constructs a new reader.
         *
         * @param microBit the micro:bit instance.
         */
        explicit WireReader(MicroBit *microBit);

        /**
         * Listens for a packet has been received.
         *
         * @tparam T the class type.
         * @param object the object.
         * @param handler the objects member function, handles the event.
         */
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
                uint16_t packetLength = readShort();
                vector<uint8_t> payload = readAll(packetLength);
                (object->*handler)(payload);
                microBit->sleep(MORSE_CODE_TICK_RATE);
            }
        }
    };
}

#endif //MORSE_CODE_WIRE_READER_H
