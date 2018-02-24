#include "SpeckCipher.h"

#include <cstring>

// Configuration.
#define ROUNDS 22
#define KEY "XnvKm5nMaN8vaTkDK6PuwnEWDzHG6t9W" \
            "pcQZRyRufMzHKT3kSsjrxRF2P8F5eB9b" \
            "P6NhDWttkcAvfWDakNCkCnLktNnapngN" \
            "RmnC25PUz27KUzRTvvMyuYyRcqqBYn7"

// ---------------------------- //
//  Rotates for 16 bit integers //
// ---------------------------- //

// Rotate left 16 bit integer 'r' times.
#define ROTATE_LEFT(x, r) (((x) >> (r)) | ((x) << 16 - (r)))

// Rotate right 16 bit integer 'r' times.
#define ROTATE_RIGHT(x, r) (((x) << (r)) | ((x) >> 16 - (r)))

// ---------------------------- //
//  Encrypt and Decrypt Rounds  //
// ---------------------------- //

// X = Rotate Left 7, Add Y, XOR Key.
// Y = Rotate Right 2, XOR X.
#define ENCRYPT(x, y, k) ((x) = ROTATE_LEFT((x), 7), (x) += (y), (x) ^= (k), (y) = ROTATE_RIGHT(y, 2), (y) ^= (x))

// Y = XOR X, Rotate Left 2.
// X = XOR Key, Negate Y, Rotate Right 7.
#define DECRYPT(x, y, k) ((y) ^= (x), (y) = ROTATE_LEFT(y, 2), (x) ^= (k), (x) -= (y), (x) = ROTATE_RIGHT(x, 7))

namespace morse_code {

    void SpeckCipher::encryptBlock(uint16_t *block) {
        block[0] ^= initializationVectorX;
        block[1] ^= initializationVectorY;

        for (uint16_t i = 0; i < ROUNDS; i++) {
            ENCRYPT(block[1], block[0], ((uint16_t *) KEY)[i]);
        }

        initializationVectorX = block[0];
        initializationVectorY = block[1];
    }

    void SpeckCipher::decryptBlock(uint16_t *block) {
        uint16_t x = block[0];
        uint16_t y = block[1];

        for (int32_t i = (ROUNDS - 1); i >= 0; i--) {
            DECRYPT(block[1], block[0], ((uint16_t *) KEY)[i]);
        }

        block[0] ^= initializationVectorX;
        block[1] ^= initializationVectorY;
        initializationVectorX = x;
        initializationVectorY = y;
    }

    std::vector<uint8_t> SpeckCipher::encrypt(const std::vector<uint8_t> &plainText) {
        // Copy the plain text into a buffer with room for padding.
        auto padding = static_cast<uint8_t>(4 - (plainText.size() % 4) & 3);
        size_t bufferSize = plainText.size() + padding;
        uint8_t buffer[bufferSize] = {0};
        std::memcpy(&buffer, plainText.data(), plainText.size() * sizeof(uint8_t));

        // Adds PKCS7 padding to a buffer that is not divisible by the current
        // encryption algorithms block size. In this case, our block size is 32 bits.
        // https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7
        for (uint64_t i = plainText.size(); i < bufferSize; i++) {
            buffer[i] = padding;
        }

        // Encrypt each block in succession.
        for (size_t i = 0; i < bufferSize / 2; i += 2) {
            encryptBlock(reinterpret_cast<uint16_t *>(buffer) + i);
        }

        // Return the cipher text.
        std::vector<uint8_t> cipherText;
        cipherText.assign(buffer, buffer + bufferSize);
        return cipherText;
    }

    std::vector<uint8_t> SpeckCipher::decrypt(const std::vector<uint8_t> &cipherText) {
        // Copy the cipher text into a buffer.
        uint8_t buffer[cipherText.size()];
        std::memcpy(&buffer, cipherText.data(), cipherText.size() * sizeof(uint8_t));

        // Decrypt each block in succession.
        for (size_t i = 0; i < cipherText.size() / 2; i += 2) {
            decryptBlock(reinterpret_cast<uint16_t *>(buffer) + i);
        }

        // Return the plain text.
        std::vector<uint8_t> plainText;
        plainText.assign(buffer, buffer + cipherText.size());
        return plainText;
    }
}
