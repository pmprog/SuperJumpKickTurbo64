#!/bin/bash
rm sjkt.prg
rm sjkt.s
../CompileOnly/cc65/bin/cc65 -t c64 -I ../CompileOnly/cc65/include -Ors -o sjkt.s sjkt.c
../CompileOnly/cc65/bin/ca65 -t c64 -I ../CompileOnly/cc65/include -o sjkt.o sjkt.s
../CompileOnly/cc65/bin/ld65 -o sjkt.prg -t c64 -m sjkt.map sjkt.o c64.lib
rm sjkt.o
rm sjkt.map

if [ -f sjkt.prg ]; then
  x64 -autostart sjkt.prg
fi
