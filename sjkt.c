
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
	
	SPRPTR.spr0 = 240; // 192;
	SPRPTR.spr1 = 241; // 193;
	SPRPTR.spr2 = 242; // 194;
	SPRPTR.spr3 = 243; // 195;
	SPRPTR.spr4 = 244;
	SPRPTR.spr5 = 245;
	SPRPTR.spr6 = 246;
	SPRPTR.spr7 = 247;
	
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

