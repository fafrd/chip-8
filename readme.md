# chip-8 emulator

[CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) interpreter written in C

(technically, it's not an emulator, it's just an interpreter, but you know...)

## building
using ubuntu 16.04...

get your build tools

    sudo apt-get install build-essential libncurses5-dev

clone and build

    git clone https://github.com/fafrd/chip-8.git
    cd chip-8
    make
    
## running

run a game

    ./chip8 run roms/pong.rom

run a game at a specific speed (expressed in cycles/second. default 300hz)

	./chip8 run roms/pong.rom -s 10
    
disassemble a game (print instructions using [Cowgod's](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM) syntax)

    ./chip8 disassemble roms/pong.rom | less

## controls
Game controls

	chip keys                               keyboard keys
	---------                               -------------
	 1 2 3 C                                   1 2 3 4
 	 4 5 6 D    This is emulated with these    Q W E R
	 7 8 9 E    keyboard keys -->              A S D F
	 A 0 B F                                   Z X C V

Emulation controls

      esc             exit program
      space           pause/resume emulation
    / slash           step emulation
    , comma           reduce emulation speed
    . period          increase emulation speed
    ; semicolon       toggle live printing of instructions
    ' apostrophe      toggle live display of registers
    [  left bracket   set shift result quirk (affects instructions 8xy6, 8xye)
    ]  right bracket  set i register quirk (affects instructions fx55, fx65)

## testing

run the unit tests with

	./test

## todo
see the [todo](todo) file


## helpful resources
- intro to chip-8 (helpful for getting started) [www.emulator101.com/introduction-to-chip-8.html](http://www.emulator101.com/introduction-to-chip-8.html)
- in-depth reference [mattmik.com/files/chip8/mastering/chip8.html](http://mattmik.com/files/chip8/mastering/chip8.html)
- quick reference [devernay.free.fr/hacks/chip8/C8TECH10.HTM](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- good reference emulator for testing roms [www.newgrounds.com/portal/view/632197](https://www.newgrounds.com/portal/view/632197)
