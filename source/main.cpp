#include <string>
#include <MicroBit.h>
#include "WireReader.h"
#include "WireWriter.h"

MicroBit microBit;

void packet_listener(std::vector<char> packet) {
    microBit.display.scroll(ManagedString(packet.data(), (uint16_t) packet.size()));
}

int main() {
    microBit.init();

    auto reader = new WireReader(&microBit);
    reader->listen(packet_listener);

//    auto writer = new WireWriter(&microBit);
//    writer->wait();
//    while (true) {
//        writer->write("hi");
//    }
    return EXIT_SUCCESS;
}
