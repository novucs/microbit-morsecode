#ifndef MORSECODE_WIREWRITER_H
#define MORSECODE_WIREWRITER_H

#include <string>
#include <MicroBit.h>

class WireWriter {
private:
    MicroBit *microBit;

    void writeBit(int bit);

    void writeByte(char byte);

    void writeShort(short value);

public:
    explicit WireWriter(MicroBit *microBit);

    void write(std::vector<char> packet);

    void write(std::string packet);

    void wait();
};

#endif // MORSECODE_WIREWRITER_H
