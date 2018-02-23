#include <vector>
#include "WireWriter.h"
#include "WireConfig.h"

WireWriter::WireWriter(MicroBit *microBit, MicroBitPin *pin) : microBit(microBit), pin(pin) {}

void WireWriter::writeBit(int bit) {
    pin->setDigitalValue(bit > 0);
    microBit->sleep(TICK_RATE);
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

void WireWriter::wait() {
    while (pin->getDigitalValue() == 0) {
        microBit->sleep(TICK_RATE);
    }
    while (pin->getDigitalValue() == 1) {
        microBit->sleep(TICK_RATE);
    }
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

void WireWriter::ping() {
    writeBit(false);
    writeBit(true);
    writeBit(false);
}
