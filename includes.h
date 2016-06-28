
#ifndef INCLUDES_H
#define INCLUDES_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <c64.h>

#define SCREEN1						0xE000
#define SCREEN2						0xE400
#define CHARSET						0xE800
#define outb(addr,val)		(*(addr)) = (val)
#define inb(addr)					(*(addr))

struct Box
{
	unsigned int xpos;
	unsigned int ypos;
	unsigned int width;
	unsigned int height;
};

#endif