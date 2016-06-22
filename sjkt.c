
#include "includes.h"

#include "tools.h"
#include "sprites.h"
#include "polymath.h"
#include "menu.h"
#include "fight.h"

int main(void)
{
	int menuoption;
	
	clrscr();
	bordercolor (COLOR_BLACK);
	bgcolor (COLOR_BLACK);
	textcolor (COLOR_WHITE);
	
	SPRPTR.spr0 = 192;
	SPRPTR.spr1 = 193;
	SPRPTR.spr2 = 194;
	SPRPTR.spr3 = 195;
	SPRPTR.spr4 = 196;
	SPRPTR.spr5 = 197;
	SPRPTR.spr6 = 198;
	SPRPTR.spr7 = 199;
	
	polyintro();
	
	while( 0 == 0 )
	{
		menuoption = menuscreen();
		
		switch( menuoption )
		{
			case 0:
				fightstage( 0, 1 );
				break;
			case 1:
				fightstage( 0, 0 );
				break;
			case 2:
				fightstage( 1, 1 );
				break;
		}
	}
	
	return 0;
}

