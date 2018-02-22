#include <vector>
#include "WireWriter.h"
#include "WireConfig.h"

WireWriter::WireWriter(MicroBit *microBit) : microBit(microBit) {}

void WireWriter::writeBit(int bit) {
    microBit->io.P1.setDigitalValue(bit > 0);
    microBit->sleep(TICK_RATE);
}

void WireWriter::writeByte(char byte) {
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
    writeByte((char) (value >> 8));
    writeByte((char) (value & 0xFF));
}

void WireWriter::wait() {
    while (microBit->io.P1.getDigitalValue() == 0) {}
    while (microBit->io.P1.getDigitalValue() == 1) {}
    microBit->sleep(500);
}

void WireWriter::write(std::vector<char> packet) {
    writeBit(true);
    writeShort((short) packet.size());
    for (char byte : packet) {
        writeByte(byte);
    }
    writeBit(true);
    writeBit(false);
}

void WireWriter::write(std::string packet) {
    writeBit(true);
    writeShort((short) packet.size());
    for (char byte : packet) {
        writeByte(byte);
    }
    writeBit(true);
    writeBit(false);
}
