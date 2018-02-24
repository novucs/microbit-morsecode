#include "WireReader.h"

namespace morse_code {

    WireReader::WireReader(MicroBit *microBit) : microBit(microBit) {
        // Bind reading to pin 2.
        WireReader::pin = &microBit->io.P2;
        WireReader::pinId = MICROBIT_ID_IO_P2;
    }

    void WireReader::onByte(uint8_t byte) {
        // Reset the cached bits.
        bits = 0;
        bitsLength = 0;

        // Push to the newly read bytes queue.
        bytes.push(byte);

        switch (state) {
            case READ_LENGTH: {
                // Consume two bytes and set the packet length.
                static uint8_t last_byte = 0;
                readLength += 1;
                if (readLength == 2) {
                    readLength = 0;
                    packetLength = (last_byte << 4) + byte;
                    state = READ_PAYLOAD;
                }
                last_byte = byte;
                break;
            }
            case READ_PAYLOAD: {
                // Consume all bytes until whole packet is read in.
                // Update state to be ready for the next packet.
                readLength += 1;
                if (readLength == packetLength) {
                    ignoreBits = 3;
                    readLength = 0;
                    state = READ_LENGTH;
                }
                break;
            }
            default: {
                break;
            }
        }
    }

    void WireReader::onHi(MicroBitEvent event) {
        // Consume first bit on initial connection.
        if (state == FIRST) {
            state = READ_LENGTH;
            return;
        }

        // Get number of ticks line has been HI.
        uint64_t ticks = (event.timestamp / 1000) / MORSE_CODE_TICK_RATE;

        for (uint64_t i = 0; i < ticks; i++) {
            // Consume all ignored bits.
            if (ignoreBits > 0) {
                ignoreBits--;
                continue;
            }

            // Update bit cache with new value.
            bits <<= 1;
            bits += 1;
            bitsLength += 1;

            // Call byte handler once a whole byte has been read in.
            if (bitsLength == 8) {
                onByte(bits);
            }
        }
    }

    void WireReader::onLo(MicroBitEvent event) {
        // Do nothing when connection has not been setup.
        if (state == FIRST) {
            return;
        }

        // Get number of ticks line has been LO.
        uint64_t ticks = (event.timestamp / 1000) / MORSE_CODE_TICK_RATE;

        for (uint64_t i = 0; i < ticks; i++) {
            // Consume all ignored bits.
            if (ignoreBits > 0) {
                ignoreBits = 0;
                state = FIRST;
                return;
            }

            // Update bit cache with new value.
            bits <<= 1;
            bitsLength += 1;

            // Call byte handler once a whole byte has been read in.
            if (bitsLength == 8) {
                onByte(bits);
            }
        }
    }

    std::vector<uint8_t> WireReader::readAll(int length) {
        // Read all new bytes for the specified length.
        std::vector<uint8_t> target;

        while (true) {
            while (bytes.size() > 0 && length > 0) {
                const uint8_t byte = bytes.front();
                bytes.pop();
                target.push_back(byte);
                length -= 1;
            }

            if (length <= 0) {
                return target;
            }

            microBit->sleep(MORSE_CODE_TICK_RATE);
        }
    }

    uint16_t WireReader::readShort() {
        // Reads two bytes and creates a short.
        auto bytes = readAll(2);
        return (bytes[0] << 4) + bytes[1];
    }
}
