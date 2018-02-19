# microbit-platformer

A simple platform game written for the micro:bit.

## Building

Modify `Makefile` in the root directory of the project to ensure the
`MICROBIT_PATH` is set to where ever your micro:bit is mounted.

In ArchLinux, this will be:

`/run/media/<username>/MICROBIT`

If attempting to run on the UWE machines, the micro:bit should be
mounted at:

`/media/student/MICROBIT`.

Once properly configured, you may build the project and copy the binary
onto the device all with the `make` command.

## Usage

On first boot, you will be taken to the main menu where you will be
asked to select which world to play. All tediously long messages in this
game may be skipped with the A button. Once the text has scrolled or
been skipped, a number should appear on your screen. This number
identifies the world currently selected. You may simply tilt the
micro:bit left or right to relatively increase or decrease your
selection. Higher level worlds are larger and more difficult.

### Single player

Simply select a world from the main menu by tilting and pressing the B
button to confirm your choice. You should now find yourself in the game.
The one very bright LED is your character, the static dim LEDs are the
blocks, the flashing LEDs are coins. The aim of the game is to get to
the end of the world (right most position) by moving and jumping while
picking up as many coins as possible.

Tilt the micro:bit left and right to move the character left and right.
Pressing the B button will make the character jump. Pressing the A
button will return back to the main menu. While moving, your view of the
world will scroll to your new position.

#### Worlds
Only a small portion of the world is rendered at any point in time due
to the size limitations of the LED matrix. Here you may find the
overview of all the worlds.

Falling off a platform to the underside of the world causes death and
the screen will flash. Your score will be reset, all coins will be
spawned back in and you will be placed in the starting position. You
have unlimited lives until the level is complete. Keep playing until
you collect all coins!

##### Key
` ` = Air

`█` = Solid block

`▒` = Flag (map finish line)

`.` = Coin

`x` = Starting position

##### World 1
```

 .       .
███    .   .      ▒
                 ▒▒
      █████       ▒
 x .      ████    ▒
█████        ███████
```

##### World 2
```
         .
             .
                  ▒
    .   ███      ▒▒
     ██           ▒
 x.         ██    ▒
████            ████
```

##### World 3
```
         ....█
         █████..
████         █████    ███
    . .      █..
             █..       .  ██
     ███ . . ███     .                  ▒
  ████                  ██ .     .     ▒▒
    .█   ████     ██       .            ▒
 x   █ ..        ██        .   █████    ▒
█████████    █████        ████       █████
```

##### World 4
```
 .               .
  .      .                     .
 .   ███ .       .      .
 ██  █   █                     █  .
  ██ █          ███         . ██
     █   .  ███     ███  █  .     .     ▒
    ██   .                  .          ▒▒
         █                  █     .     ▒
 x          .               ██   ███    ▒
████████    █                ██       ████
```

##### World 5
```
                    █..
 .                  █..
 .                  █..
 .                  ███  .
 .                  █ ██
 █   .    .    .    █    .   .   .      ▒
   .              █████            .   ▒▒
     █    █    █         █   █   █      ▒
 x                . . .                 ▒
████             ██ █ ██             █████
```

### Multiplayer

Press both A and B at the same time while in the main menu. You will be
taken to the connection screen, where your device will search for other
nearby devices that are also on this screen. Once an opponent device has
been detected, they will automatically connect. A happy smiley face
should be displayed on successful connection. In the event you no longer
wish to wait for an opponent and want to continue to single player,
press the A button. A sad face will play to indicate no connection has
been made.

Once connected, you will be taken back to the main menu where you may
choose to disconnect by pressing A and B at the same time. To play a
game in multiplayer, select a world. The first world selected (by either
player) will be the world that is played.

The controls and the worlds are exactly the same as single player,
except now you are aiming to complete this map with the most amount of
coins before the other player gets there. Each of you have unlimited
lives until one person completes the level. After one person completes
the level, the remaining player only has one life. If they die, the
first person to complete the level automatically wins. If both players
complete the level, the player with the highest score wins.
