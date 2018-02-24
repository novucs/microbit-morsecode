#include <string>
#include "MorseCode.h"

int main() {
    auto *morseCode = new morse_code::MorseCode();
    morseCode->run();
    delete morseCode;
    return EXIT_SUCCESS;
}
