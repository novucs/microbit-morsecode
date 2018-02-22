#include "WireReader.h"
#include "WireConfig.h"

WireReader::WireReader(MicroBit *microBit) : microBit(microBit) {}

void WireReader::onByte(char byte) {
    bits = 0;
    bitsLength = 0;
    bytes.push(byte);

    static char last_byte = 0;
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

    uint64_t ticks = (event.timestamp / 1000) / TICK_RATE;

    for (int i = 0; i < ticks; i++) {
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

    uint64_t ticks = (event.timestamp / 1000) / TICK_RATE;

    for (int i = 0; i < ticks; i++) {
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

std::vector<char> WireReader::readAll(int length) {
    std::vector<char> target;

    while (true) {
        while (bytes.size() > 0 && length > 0) {
            const char byte = bytes.front();
            bytes.pop();
            target.push_back(byte);
            length -= 1;
        }

        if (length <= 0) {
            return target;
        }

        microBit->sleep(TICK_RATE);
    }
}

short WireReader::readShort() {
    auto bytes = readAll(2);
    return (bytes[0] << 4) + bytes[1];
}

void WireReader::listen(void (*handler)(std::vector<char>)) {
    microBit->io.P2.setDigitalValue(1);
    microBit->sleep(500);
    microBit->io.P2.setDigitalValue(0);

    microBit->io.P2.eventOn(MICROBIT_PIN_EVENT_ON_PULSE);
    microBit->messageBus.listen(MICROBIT_ID_IO_P2, MICROBIT_PIN_EVT_PULSE_HI, this, &WireReader::onHi,
                                MESSAGE_BUS_LISTENER_IMMEDIATE);
    microBit->messageBus.listen(MICROBIT_ID_IO_P2, MICROBIT_PIN_EVT_PULSE_LO, this, &WireReader::onLo,
                                MESSAGE_BUS_LISTENER_IMMEDIATE);

    listening = true;

    while (listening) {
        short packet_length = readShort();
        std::vector<char> payload = readAll(packet_length);
        handler(payload);
        microBit->sleep(TICK_RATE);
    }
}
