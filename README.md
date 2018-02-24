# microbit-morsecode

An encrypted micro:bit messaging system using GPIOs.

## Building

Modify `Makefile` in the root directory of the project to ensure the
`MICROBIT_PATH` is set to where ever your micro:bit is mounted.

In ArchLinux, this will be:

`/run/media/<username>/MICROBIT`

If attempting to run on the UWE machines, the micro:bit should be
mounted at:

`/media/student/MICROBIT`.

Once properly configured, you may build the project and copy the binary
onto both devices with the `make` command.

*Note: Both devices must be plugged in to do this. To install on a
       single plugged in device, use `make 0`*

## Usage

Before booting the devices, connect crocodile clip wires from GPIO pins
1 to 2 on each device. Pin 1 handles sending and pin 2 handles reading.
Feel free to use either one or two wires, depending whether you would
like a one-way or two-way connection.

As soon as the device is booted, the message "*MORSE CODE*" should
scroll on the screen. You may now begin sending messages.

To send text messages, input may be provided as morse code. Tap the A
button to enter a dot, hold the A button for a little longer to enter a
dash. Wait for a second between entering each character. Once a
character has been entered, it should be displayed on the screen. When
you are finished entering all characters for the message, send it by
pressing the B button. Messages take time to send as this protocol has
very low bandwidth (`~28bits/second`).

Two emojis are also supported by this protocol, smiley and sad faces. To
send a happy face, press both A and B buttons at the same. To send a sad
face, shake the device.

### Morse Code Chart

#### Key

`.` = Dot

`■` = Dash

```
. ■         A
■ . . .     B
■ . ■ .     C
■ . .       D
.           E
. . ■ .     F
■ ■ .       G
. . . .     H
. .         I
. ■ ■ ■     J
■ . ■       K
. ■ . .     L
■ ■         M
■ .         N
■ ■ ■       O
. ■ ■ .     P
■ ■ . ■     Q
. ■ .       R
. . .       S
■           T
. . ■       U
. . . ■     V
. ■ ■       W
■ . . ■     X
■ . ■ ■     Y
■ ■ . .     Z
. ■ ■ ■ ■   1
. . ■ ■ ■   2
. . . ■ ■   3
. . . . ■   4
. . . . .   5
■ . . . .   6
■ ■ . . .   7
■ ■ ■ . .   8
■ ■ ■ ■ .   9
■ ■ ■ ■ ■   0
```

## How it works

Although messages are entered as morse code, this protocol does not use
it for sending data as it is not secure. It is possible to encrypt the
message and encode it in morse code, but the transfer speeds would be
much slower than they already are. Therefore I decided to create a
packet sending framework to add good encryption without sacrificing
precious bandwidth. Packets are identified through the power-efficient
HI and LO GPIO events provided by the micro:bit DAL API. When parsing
these events, each bit is represented by a HI or LO signal for `35
milliseconds`. The program attempts to identify the following packet
structure:

```
START
- HI, 1 bit

LENGTH
- Short (2 bytes)

PAYLOAD
- Variable length in bytes, indicated by the length first read.
- Maximum size is 65535 bytes. The micro:bit will run out of memory
  before being able to read it all in (micro:bits have 16KB RAM). So
  this should be plenty of data to send.
- All this conetent should be encrypted using SpeckCipher.

END
- HI, 1 bit
- LO, permanent until next packet
```

A lot of research was put into finding the best encryption algorithm for
this protocol, AES128 bit encryption seemed to be the best candidate for
a while until I found 32bit SpeckCipher. Of course, the encryption is
deemed less secure than AES128 but that does not mean it is insecure. In
my eyes, the positives of using 32bit SpeckCipher far outweigh the
negatives. The two huge positives that swayed me to do this were:

1. The amount of data we are required to transfer must be minimal for
   this protocol. Using AES128bit encryption must send data in multiples
   of 128bits, this means even sending the smallest packet would take
   `~4.5 seconds`! Which is a huge duration as compared to the
   `~1.1 seconds` provided by 32bit SpeckCipher.
2. Power efficiency is key in IoT devices such as the micro:bit, and
   this cipher was designed specifically for IoT devices as a secure yet
   power efficient alternative to AES128bit encryption. The relevant
   research paper can be viewed here:
   [(2015) Simon and Speck: Block Ciphers for the Internet of Things](
   https://csrc.nist.gov/csrc/media/events/lightweight-cryptography-workshop-2015/documents/papers/session1-shors-paper.pdf)

It is possible to support larger block sizes for better encryption at
the cost of bandwidth by simply toggling a couple of cipher hyper-
parameters. I've stuck with a block size of 32bit to ensure maximum
responsiveness.

SpeckCipher or AES alone are not secure for this protocol, as we will be
susceptible to replay attacks. By default just implementing a cipher is
in Electronic Code Book (ECB) mode. Encrypting the same message in ECB
always results in the same cipher text. To counteract this, I've chosen
to use Cipher Block Chaining (CBC) which XORs the next input by an
initialization vector (IV). Each block processed then updates the IV.
More information can be found on this [here](
https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#Cipher_Block_Chaining_(CBC)).

The morse code messaging protocol built on top of this is very simple.
The first byte represents the packet type, which is one of the
following:

* MESSAGE
* HAPPY
* SAD

Message packet types are then followed by a sequence of characters that
are displayed to the screen. Happy and sad packets contain no extra
information as they're only used to tell a device to display an emoji.
