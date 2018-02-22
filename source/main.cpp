#include <string>
#include <MicroBit.h>
#include "WireReader.h"
#include "WireWriter.h"

MicroBit microBit;
bool running = true;

void packet_listener(std::vector<char> packet) {
    microBit.display.scroll(ManagedString(packet.data(), (uint16_t) packet.size()));
}

void read() {
    auto reader = new WireReader(&microBit, &microBit.io.P2);
    reader->listen(packet_listener);
}

void write() {
    auto writer = new WireWriter(&microBit, &microBit.io.P1);
    writer->wait();
    while (running) {
        writer->write("hi");
        microBit.sleep(200);
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
