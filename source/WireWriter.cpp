#include "WireWriter.h"
#include "MorseCode.h"

namespace morse_code {

    WireWriter::WireWriter(MicroBit *microBit) : microBit(microBit) {
        WireWriter::pin = &microBit->io.P1;
    }

    void WireWriter::writeBit(int bit) {
        pin->setDigitalValue(bit > 0);
        microBit->sleep(MORSE_CODE_TICK_RATE);
    }

    void WireWriter::writeByte(uint8_t byte) {
        writeBit(byte & 0x80);
        writeBit(byte & 0x40);
        writeBit(byte & 0x20);
        writeBit(byte & 0x10);
        writeBit(byte & 0x08);
        writeBit(byte & 0x04);
        writeBit(byte & 0x02);
        writeBit(byte & 0x01);
    }

    void WireWriter::writeShort(short value) {
        writeByte((uint8_t) (value >> 8));
        writeByte((uint8_t) (value & 0xFF));
    }

    void WireWriter::write(std::vector<uint8_t> packet) {
        writeBit(true);
        writeShort((short) packet.size());

        for (uint8_t byte : packet) {
            writeByte(byte);
        }

        writeBit(true);
        writeBit(false);
    }
}
