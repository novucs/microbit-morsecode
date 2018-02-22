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
    ReadState state = FIRST;
    std::queue<char> bytes;
    MicroBit *microBit;
    bool listening = false;
    char bits = 0;
    int bitsLength = 0;
    int packetLength = 0;
    int readLength = 0;
    int ignoreBits = 0;

    vector<char> readAll(int length);

    short readShort();

    void onLo(MicroBitEvent event);

    void onHi(MicroBitEvent event);

    void onByte(char byte);

public:
    explicit WireReader(MicroBit *microBit);

    void listen(void (*handler)(std::vector<char>));

};


#endif //MORSECODE_WIREREADER_H
