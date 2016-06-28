
#include "includes.h"

struct __spritepointers {
	unsigned char	spr0;
	unsigned char	spr1;
	unsigned char	spr2;
	unsigned char	spr3;
	unsigned char	spr4;
	unsigned char	spr5;
	unsigned char	spr6;
	unsigned char	spr7;
};

#define SPRPTR			(*(struct __spritepointers*)0x07F8)

/* Define special memory areas */
#define SCREEN_MAP	((unsigned char*)0x0400)
#define VIC_MAP			((unsigned char*)0xD000)

#define JOY1				((unsigned char*)0xDC01)
#define JOY2				((unsigned char*)0xDC00)
#define JOY_UP			1
#define JOY_DOWN		2
#define JOY_LEFT		4
#define JOY_RIGHT		8
#define JOY_FIRE		16


void waitframes(int frames)
{
	unsigned char delay;
	for( delay = 0; delay < frames; ++delay )
	{
		while( VIC.rasterline > 0 )
		{
		}
	}
}

void waitframesorbutton(int frames)
{
	unsigned char delay;
	for( delay = 0; delay < frames; ++delay )
	{
		while( VIC.rasterline > 0 )
		{
		}
		if( (JOY2[0] & JOY_FIRE) == 0 || (JOY1[0] & JOY_FIRE) == 0 )
		{
			return;
		}
	}
}

void setspritexy(int spritenum, int x, unsigned char y)
{
	if( x < 0 )
	{
		x = 0;
	}

	if( x >= 256 )
	{
		VIC_MAP[spritenum * 2] = (unsigned char)(x - 256);
		VIC.spr_hi_x = VIC.spr_hi_x | (1 << spritenum);
	} else {
		VIC_MAP[spritenum * 2] = (unsigned char)x;
		VIC.spr_hi_x = VIC.spr_hi_x & ~(1 << spritenum);
	}
	VIC_MAP[(spritenum * 2) + 1] = y;
}

int getspritex(int spritenum)
{
	int xpos = VIC_MAP[spritenum * 2];
	if( (VIC.spr_hi_x & (1 << spritenum)) != 0 )
	{
		xpos += 256;
	}
	return xpos;
}

void coloursprites(unsigned char colour)
{
	VIC.spr0_color = colour;
	VIC.spr1_color = colour;
	VIC.spr2_color = colour;
	VIC.spr3_color = colour;
	VIC.spr4_color = colour;
	VIC.spr5_color = colour;
	VIC.spr6_color = colour;
	VIC.spr7_color = colour;
}

/*
unsigned char arespritesoverlapping(unsigned char spra, unsigned char sprb)
{
	int sax = getspritex( spra );
	int sbx = getspritex( sprb );
	unsigned char say = VIC_MAP[(spra * 2) + 1];
	unsigned char sby = VIC_MAP[(sprb * 2) + 1];
	
	if( sax < sbx + 48 && sax + 48 >= sbx )
	{
		if( say < sby + 42 && say + 42 >= sby )
		{
			return 1;
		}
	}
	return 0;
}
*/

unsigned char doboxesoverlay(struct Box* boxa, struct Box* boxb)
{
	if( boxa->xpos < boxb->xpos + boxb->width && boxa->xpos + boxa->width >= boxb->xpos )
	{
		if( boxa->ypos < boxb->ypos + boxb->height && boxa->ypos + boxa->height >= boxb->ypos )
		{
			return 1;
		}
	}
	return 0;
}

