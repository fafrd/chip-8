to create a rom, use the following command:

`echo -n 625a 8320 0033 | tr -d '[:space:]' | xxd -r -p > roms/test.rom`
