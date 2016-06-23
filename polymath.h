
#include "includes.h"

static const unsigned char logofader[5] = { 
	COLOR_BLACK, COLOR_GRAY1, COLOR_GRAY2, COLOR_GRAY3, COLOR_WHITE
};

void writepolytext()
{
	gotoxy( 10, 16 );
	printf("Polymath Programming");
	gotoxy( 12, 18 );
	printf("www.pmprog.co.uk");
}

void polyintro(void)
{
	unsigned char idx;
	
	memcpy((void*)0x3C00, &SpriteLogo, 0x100);
	
	VIC.spr0_x = 160;
	VIC.spr0_y = 100;
	VIC.spr1_x = 184;
	VIC.spr1_y = 100;
	VIC.spr2_x = 160;
	VIC.spr2_y = 121;
	VIC.spr3_x = 184;
	VIC.spr3_y = 121;
	
	for( idx = 0; idx < 5; ++idx )
	{
		textcolor( logofader[idx] );
		writepolytext();
		coloursprites(logofader[idx]);
		VIC.spr_ena = 0xFF;
		waitframes(10);
	}
	
	waitframes(100);
	
	for( idx = 5; idx > 0; --idx )
	{
		textcolor( logofader[idx - 1] );
		writepolytext();
		coloursprites(logofader[idx - 1]);
		waitframes(10);
	}
	VIC.spr_ena = 0x00;
}
