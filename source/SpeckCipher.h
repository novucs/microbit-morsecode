#ifndef PLATFORMER_SPECK_CIPHER_H
#define PLATFORMER_SPECK_CIPHER_H

#include <cstdint>
#include <vector>

/**
 * Implementation of Speck Cipher, a lightweight block cipher designed for IoT
 * devices. More information can be found here:
 *
 * @see <a href="https://csrc.nist.gov/csrc/media/events/lightweight-cryptography-workshop-2015/documents/papers/session1-shors-paper.pdf">
 *  Simon and Speck: Block Ciphers for the Internet of Things
 * </a>
 *
 * Written with the following properties:
 * - 64 bit key.
 * - 32 bit block.
 * - 22 rotations.
 * - 7 and 2 bit rotations due to a 16 bit word size.
 *  (Larger word sizes use 8 and 3 bit rotations, respectively)
 * - Cipher Block Chaining (CBC) to overcome replay attacks.
 *
 * A small block size was chosen as the throughput of the transfer protocol is
 * very low, thus we need to send the smallest amount of data as possible to
 * ensure maximum response time.
 */
class SpeckCipher {
private:

    // Initialization vector X and Y values.
    // Used for Cipher Block Chaining.
    uint16_t initializationVectorX = 0x0420;
    uint16_t initializationVectorY = 0x1337;

    /**
     * Encrypts a single block, the encrypted contents will be placed back on
     * the block provided.
     *
     * @param block the block to encrypt.
     */
    void encryptBlock(uint16_t *block);

    /**
     * Decrypts a single block, the decrypted contents will be placed back on
     * the block provided.
     *
     * @param block the block to decrypt.
     */
    void decryptBlock(uint16_t *block);

public:

    /**
     * Encrypts a message using Speck Cipher with Cipher Block Chaining (CBC).
     * Automatically pads the plain text using PKCS7.
     *
     * @param plainText the plain text to encrypt.
     * @return the cipher text.
     */
    std::vector<uint8_t> encrypt(std::vector<uint8_t> plainText);

    /**
     * Decrypts a message using Speck Cipher with Cipher Block Chaining (CBC).
     *
     * @param cipherText the cipher text to decrypt.
     * @return the plain text.
     */
    std::vector<uint8_t> decrypt(std::vector<uint8_t> cipherText);

};

#endif //PLATFORMER_SPECK_CIPHER_H
