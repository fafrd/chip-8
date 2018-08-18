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

    ./emulator run roms/pong.rom
    
disassemble a game (print instructions using [Cowgod's](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM) syntax)

    ./emulator disassemble roms/pong.rom | less

## controls
    chip keys                               keyboard keys
    ---------                               -------------
	 1 2 3 C                                   1 2 3 4
 	 4 5 6 D    This is emulated with these    Q W E R
	 7 8 9 E    keyboard keys -->              A S D F
	 A 0 B F                                   Z X C V

## todo
see the [todo](todo) file


## helpful resources
- intro to chip-8 (helpful for getting started) [www.emulator101.com/introduction-to-chip-8.html](http://www.emulator101.com/introduction-to-chip-8.html)
- in-depth reference [mattmik.com/files/chip8/mastering/chip8.html](http://mattmik.com/files/chip8/mastering/chip8.html)
- quick reference [devernay.free.fr/hacks/chip8/C8TECH10.HTM](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- good reference emulator for testing roms [www.newgrounds.com/portal/view/632197](https://www.newgrounds.com/portal/view/632197)
