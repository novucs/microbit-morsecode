#include <string>
#include <MicroBit.h>
#include "WireReader.h"
#include "WireWriter.h"
#include "SpeckCipher.h"

MicroBit microBit;
SpeckCipher cipher;
bool running = true;

void packet_listener(std::vector<uint8_t> encrypted) {
    std::vector<uint8_t> packet = cipher.decrypt(encrypted);

    for (char c : packet) {
        if (c == 0xFF) {
            microBit.display.print("A");
        } else if (c == 0xF0) {
            microBit.display.print("B");
        } else if (c == 0x0F) {
            microBit.display.print("C");
        } else if (c == 0x00) {
            microBit.display.print("D");
        }
        microBit.sleep(200);
    }
//    microBit.display.scroll(ManagedString(packet.data(), (uint16_t) packet.size()));
}

void read() {
    auto reader = new WireReader(&microBit, &microBit.io.P2);
    reader->listen(packet_listener);
}

void write() {
    auto writer = new WireWriter(&microBit, &microBit.io.P1);
    writer->wait();
    while (running) {
        microBit.display.print("A");
        unsigned char a[] = {0xFF, 0xF0, 0x0F, 0x00};
        writer->write(cipher.encrypt(std::vector<uint8_t>(a, a + sizeof(a))));
        microBit.display.print("B");
        microBit.sleep(2000);
    }
}

int main() {
    microBit.init();
    create_fiber(read);
    create_fiber(write);

    while (running) {
        microBit.sleep(1000);
    }

    return EXIT_SUCCESS;
}
