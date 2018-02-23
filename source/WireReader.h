#ifndef MORSECODE_WIREREADER_H
#define MORSECODE_WIREREADER_H

#include <queue>
#include <MicroBit.h>

enum ReadState {
    FIRST,
    READ_LENGTH,
    READ_PAYLOAD
};

class WireReader {
private:
    MicroBit *microBit;
    MicroBitPin *pin;
    ReadState state = FIRST;
    std::queue<uint8_t> bytes;
    bool listening = false;
    uint8_t bits = 0;
    int bitsLength = 0;
    int packetLength = 0;
    int readLength = 0;
    int ignoreBits = 0;

    vector<uint8_t> readAll(int length);

    short readShort();

    void onLo(MicroBitEvent event);

    void onHi(MicroBitEvent event);

    void onByte(uint8_t byte);

public:
    explicit WireReader(MicroBit *microBit, MicroBitPin *pin);

    void listen(void (*handler)(std::vector<uint8_t>));

};


#endif //MORSECODE_WIREREADER_H
