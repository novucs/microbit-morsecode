#include "WireReader.h"

namespace morse_code {

    WireReader::WireReader(MicroBit *microBit) : microBit(microBit) {
        WireReader::pin = &microBit->io.P2;
        WireReader::pinId = MICROBIT_ID_IO_P2;
    }

    void WireReader::onByte(uint8_t byte) {
        bits = 0;
        bitsLength = 0;
        bytes.push(byte);

        static uint8_t last_byte = 0;
        switch (state) {
            case READ_LENGTH: {
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
        if (state == FIRST) {
            state = READ_LENGTH;
            return;
        }

        uint64_t ticks = (event.timestamp / 1000) / MORSE_CODE_TICK_RATE;

        for (uint64_t i = 0; i < ticks; i++) {
            if (ignoreBits > 0) {
                ignoreBits--;
                continue;
            }

            bits <<= 1;
            bits += 1;
            bitsLength += 1;

            if (bitsLength == 8) {
                onByte(bits);
            }
        }
    }

    void WireReader::onLo(MicroBitEvent event) {
        if (state == FIRST) {
            return;
        }

        uint64_t ticks = (event.timestamp / 1000) / MORSE_CODE_TICK_RATE;

        for (uint64_t i = 0; i < ticks; i++) {
            if (ignoreBits > 0) {
                ignoreBits = 0;
                state = FIRST;
                return;
            }

            bits <<= 1;
            bitsLength += 1;

            if (bitsLength == 8) {
                onByte(bits);
            }
        }
    }

    std::vector<uint8_t> WireReader::readAll(int length) {
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

    short WireReader::readShort() {
        auto bytes = readAll(2);
        return (bytes[0] << 4) + bytes[1];
    }
}
