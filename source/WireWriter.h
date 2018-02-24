#ifndef MORSE_CODE_WIRE_WRITER_H
#define MORSE_CODE_WIRE_WRITER_H

#include <vector>
#include <string>
#include <MicroBit.h>

namespace morse_code {

    /**
     * Utility for sending packets over GPIO on the micro:bit.
     */
    class WireWriter {
    private:
        // The micro:bit instance.
        MicroBit *microBit;

        // The micro:bit pin to send packets over.
        MicroBitPin *pin;

        /**
         * Writes a single bit to the wire for a single tick.
         *
         * @param bit the bit to write, 0 or 1.
         */
        void writeBit(int bit);

        /**
         * Writes a byte to the wire for 8 ticks.
         *
         * @param byte the byte to write.
         */
        void writeByte(uint8_t byte);

        /**
         * Writes a short to the wire for 16 ticks.
         *
         * @param value the short to write.
         */
        void writeShort(short value);

    public:

        /**
         * Constructs a new writer.
         *
         * @param microBit the micro:bit instance.
         */
        explicit WireWriter(MicroBit *microBit);

        /**
         * Writes a packet to the wire. Packets written via this API must be
         * smaller than 65535 bytes, it's also best to notice packets should be
         * as small as possible as the throughput is roughly 28bits/second.
         *
         * @param packet the packet to write.
         */
        void write(std::vector<uint8_t> packet);

    };
}

#endif // MORSE_CODE_WIRE_WRITER_H
