
#define GAMESTATE_ROUNDIN			0
#define GAMESTATE_FIGHT				1
#define GAMESTATE_ROUNDOUT		2
#define GAMESTATE_MATCHOUT		3

#define FIGHTERSTATE_IDLE			0
#define FIGHTERSTATE_JUMPUP		1
#define FIGHTERSTATE_JUMPDOWN	2
#define FIGHTERSTATE_KICK			3
#define FIGHTERSTATE_KICKBACK	4
#define FIGHTERSTATE_HIT			5
#define FIGHTERSTATE_FLOORED	6

#define FLOORY								200

#include "includes.h"

struct Fighter {
	unsigned char	iscpu;
	unsigned char	state;
	unsigned char	statetime;
	int xpos;
	unsigned char	ypos;
	unsigned char	wins;
	unsigned char	faceleft;
	unsigned char	input;
};

struct GameData {
	unsigned char GameState;
	struct Fighter Player1;
	struct Fighter Player2;
};

void fightspriteinit()
{
	VIC.spr_mcolor0 = COLOR_LIGHTRED;
	VIC.spr_mcolor1 = COLOR_ORANGE;
	VIC.spr0_color = VIC.spr1_color = VIC.spr2_color = VIC.spr3_color = COLOR_YELLOW;
	VIC.spr4_color = VIC.spr5_color = VIC.spr6_color = VIC.spr7_color = COLOR_LIGHTGREEN;
	VIC.spr_ena = 0xff;
	VIC.spr_mcolor = 0xff;
}

void renderfightbackground()
{
	int idx;
	
	clrscr();
	bgcolor( COLOR_CYAN );
	for( idx = 0; idx < 440; idx++ )
	{
		SCREEN_MAP[560 + idx] = 0xa0; // menutitlechar[idx];
		COLOR_RAM[560 + idx] = COLOR_GREEN;
	}
}

void renderfighter(struct Fighter* fighter, unsigned char startsprite, unsigned char faceleft)
{
	int uploadaddr = 0x3000 + (startsprite * 0x40);
	setspritexy( startsprite + 2, 0, 0 );
	setspritexy( startsprite + 3, 0, 0 );
	switch( fighter->state )
	{
		case FIGHTERSTATE_IDLE:
			setspritexy( startsprite, fighter->xpos - 12, FLOORY - 84 );
			setspritexy( startsprite + 1, fighter->xpos - 12, FLOORY - 42 );

			if( faceleft != fighter->faceleft || fighter->statetime == 0 )
			{
				memcpy((void*)(uploadaddr), (faceleft ? &SpriteLeftIdleT : &SpriteRightIdleT ), 64);
				memcpy((void*)(uploadaddr + 0x40), (faceleft ? &SpriteLeftIdleB : &SpriteRightIdleB ), 64);
			}
			break;
		case FIGHTERSTATE_JUMPUP:
		case FIGHTERSTATE_JUMPDOWN:
		case FIGHTERSTATE_KICKBACK:
		case FIGHTERSTATE_HIT:
			setspritexy( startsprite, fighter->xpos - 12, FLOORY - fighter->ypos - 84 );
			setspritexy( startsprite + 1, fighter->xpos - 12, FLOORY - fighter->ypos - 42 );
			if( faceleft != fighter->faceleft || fighter->statetime == 0 )
			{
				memcpy((void*)(uploadaddr), (faceleft ? &SpriteLeftJumpT : &SpriteRightJumpT ), 64);
				memcpy((void*)(uploadaddr + 0x40), (faceleft ? &SpriteLeftJumpB : &SpriteRightJumpB ), 64);
			}
			break;
		case FIGHTERSTATE_KICK:
			setspritexy( startsprite, fighter->xpos - 24, FLOORY - fighter->ypos - 84 );
			setspritexy( startsprite + 1, fighter->xpos, FLOORY - fighter->ypos - 84 );
			setspritexy( startsprite + 2, fighter->xpos - 24, FLOORY - fighter->ypos - 42 );
			setspritexy( startsprite + 3, fighter->xpos, FLOORY - fighter->ypos - 42 );
			if( faceleft != fighter->faceleft || fighter->statetime == 0 )
			{
				memcpy((void*)(uploadaddr), (faceleft ? &SpriteLeftKickTL : &SpriteRightKickTL ), 64);
				memcpy((void*)(uploadaddr + 0x40), (faceleft ? &SpriteLeftKickTR : &SpriteRightKickTR ), 64);
				memcpy((void*)(uploadaddr + 0x80), (faceleft ? &SpriteLeftKickBL : &SpriteRightKickBL ), 64);
				memcpy((void*)(uploadaddr + 0xC0), (faceleft ? &SpriteLeftKickBR : &SpriteRightKickBR ), 64);
			}
			break;
		case FIGHTERSTATE_FLOORED:
			setspritexy( startsprite, fighter->xpos - 24, FLOORY - 42 );
			setspritexy( startsprite + 1, fighter->xpos, FLOORY - 42 );
			if( faceleft != fighter->faceleft || fighter->statetime == 0 )
			{
				memcpy((void*)(uploadaddr), (faceleft ? &SpriteLeftDeadL : &SpriteRightDeadL ), 64);
				memcpy((void*)(uploadaddr + 0x40), (faceleft ? &SpriteLeftDeadR : &SpriteRightDeadR ), 64);
			}
			break;
	}
	
	fighter->faceleft = faceleft;
}

void renderfight(struct GameData* data)
{
	unsigned char p1facingleft;
	
	p1facingleft = (data->Player1.xpos > data->Player2.xpos ? 1 : 0);
	
	renderfighter( &(data->Player1), 0, p1facingleft );
	renderfighter( &(data->Player2), 4, 1 - p1facingleft );
	
	// TODO: Render "Wins"
}

void renderinoverlay()
{
}

void renderoutoverlay(struct GameData* data)
{
}

void rendermatchoverlay(struct GameData* data)
{
}

void cpucontrol(struct Fighter* fighter)
{
	// TODO: AI
}

void setfighterstatus(struct Fighter* fighter, unsigned char status)
{
	fighter->status = status;
	fighter->statustime = 0;
}

void updatefighter(struct Fighter* fighter)
{
	fighter->statetime++;

	if( fighter->state == FIGHTERSTATE_IDLE )
	{
		fighter->statetime = 1;
		if( (fighter->input & JOY_UP) == 0 )
		{
			setfighterstatus( fighter, FIGHTERSTATE_JUMPUP );
		} else if( (fighter->input & JOY_FIRE) == 0 ) {
			setfighterstatus( fighter, FIGHTERSTATE_KICKBACK );
		}
	} else if( fighter->state == FIGHTERSTATE_JUMPUP || fighter->state == FIGHTERSTATE_JUMPDOWN || fighter->state == FIGHTERSTATE_KICKBACK ) {
		if( (fighter->input & JOY_FIRE) == 0 ) {
			setfighterstatus( fighter, FIGHTERSTATE_KICK );
		}
	}
}

void updatefight(struct GameData* data)
{
	if( data->Player1.iscpu == 0 )
	{
		data->Player1.input = JOY2[0];
	} else {
		cpucontrol( &data->Player1 );
	}
	updatefighter( &data->Player1 );

	if( data->Player2.iscpu == 0 )
	{
		data->Player2.input = JOY1[0];
	} else {
		cpucontrol( &data->Player2 );
	}
	updatefighter( &data->Player2 );
	
	// TODO: Collision Check
}

void fighterconfig(struct Fighter* fighter)
{
	fighter->state = FIGHTERSTATE_IDLE;
	fighter->statetime = 0;
	fighter->ypos = 0;
	fighter->wins = 0;
	fighter->input = 0;
}

void fightstage(unsigned char P1CPU, unsigned char P2CPU)
{
	struct GameData data;

	fightspriteinit();
	renderfightbackground();

	data.GameState = GAMESTATE_ROUNDIN;
	
	fighterconfig( &data.Player1 );
	data.Player1.iscpu = P1CPU;
	data.Player1.xpos = 70;
	data.Player1.faceleft = 1;
	
	fighterconfig( &data.Player2 );
	data.Player2.iscpu = P2CPU;
	data.Player2.xpos = 256;
	data.Player2.faceleft = 0;
	
	while( data.GameState != GAMESTATE_MATCHOUT )
	{
		renderfight( &data );
		switch( data.GameState )
		{
			case GAMESTATE_ROUNDIN:
				renderinoverlay();
				break;
			case GAMESTATE_ROUNDOUT:
				renderoutoverlay( &data );
				break;
			case GAMESTATE_FIGHT:
				updatefight( &data );
				break;
		}
	}
	rendermatchoverlay( &data );
}
