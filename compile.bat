@echo off
del sjkt.prg
del sjkt.s
..\bin\cc65 -t c64 -I ..\include -Ors -o sjkt.s sjkt.c
..\bin\ca65 -t c64 -I ..\include -o sjkt.o sjkt.s
..\bin\ld65 -o sjkt.prg -t c64 -m sjkt.map sjkt.o c64.lib
del sjkt.o
del sjkt.map