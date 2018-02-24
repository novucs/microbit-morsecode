#ifndef MORSECODE_WIREWRITER_H
#define MORSECODE_WIREWRITER_H

#include <string>
#include <MicroBit.h>

class WireWriter {
private:
    MicroBit *microBit;

    MicroBitPin *pin;

    void writeBit(int bit);

    void writeByte(uint8_t byte);

    void writeShort(short value);

public:
    explicit WireWriter(MicroBit *microBit, MicroBitPin *pin);

    void write(std::vector<uint8_t> packet);

};

#endif // MORSECODE_WIREWRITER_H
