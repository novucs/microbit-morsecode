#include "WireWriter.h"
#include "MorseCode.h"

namespace morse_code {

    WireWriter::WireWriter(MicroBit *microBit) : microBit(microBit) {
        // Bind writing to pin 1.
        WireWriter::pin = &microBit->io.P1;
    }

    void WireWriter::writeBit(int bit) {
        // Write the bit value and sleep for a tick.
        pin->setDigitalValue(bit > 0);
        microBit->sleep(MORSE_CODE_TICK_RATE);
    }

    void WireWriter::writeByte(uint8_t byte) {
        // Write each bit of the byte in big endian order.
        writeBit(byte & 0x80);
        writeBit(byte & 0x40);
        writeBit(byte & 0x20);
        writeBit(byte & 0x10);
        writeBit(byte & 0x08);
        writeBit(byte & 0x04);
        writeBit(byte & 0x02);
        writeBit(byte & 0x01);
    }

    void WireWriter::writeShort(uint16_t value) {
        // Write each byte in the short in big endian order.
        writeByte((uint8_t) (value >> 8));
        writeByte((uint8_t) (value & 0xFF));
    }

    void WireWriter::write(std::vector<uint8_t> packet) {
        // Disallow writing packets that are too large. Maximum size is 65kb.
        // Somewhat useless as micro:bits don't even have this much memory.
        if (packet.size() > 0xFFFF) {
            microBit->panic(420);
            return;
        }

        // Packet structure:
        // start : length : payload : end

        // Write start bit.
        writeBit(true);

        // Write packet length.
        writeShort((uint16_t) packet.size());

        // Write packet payload.
        for (uint8_t byte : packet) {
            writeByte(byte);
        }

        // Write end bit.
        writeBit(true);
        writeBit(false);
    }
}
