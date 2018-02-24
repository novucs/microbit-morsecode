#ifndef MORSECODE_WIREWRITER_H
#define MORSECODE_WIREWRITER_H

#include <vector>
#include <string>
#include <MicroBit.h>

namespace morse_code {

    class WireWriter {
    private:
        MicroBit *microBit;

        MicroBitPin *pin;

        void writeBit(int bit);

        void writeByte(uint8_t byte);

        void writeShort(short value);

    public:
        explicit WireWriter(MicroBit *microBit);

        void write(std::vector<uint8_t> packet);

    };
}

#endif // MORSECODE_WIREWRITER_H
