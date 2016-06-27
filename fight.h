
#define GAMESTATE_ROUNDIN			0
#define GAMESTATE_FIGHT				1
#define GAMESTATE_ROUNDOUT		2
#define GAMESTATE_MATCHOUT		3

#define FIGHTERSTATE_IDLE			0
#define FIGHTERSTATE_JUMP			1
#define FIGHTERSTATE_KICK			2
#define FIGHTERSTATE_KICKBACK	3
#define FIGHTERSTATE_HIT			4
#define FIGHTERSTATE_FLOORED	5

#define FLOORY								240
#define FIGHTER_CPU_MAXDELAY	4

#include "includes.h"

struct Fighter {
	unsigned char	iscpu;
	unsigned char	state;
	unsigned char	statetime;
	int xpos;
	char	ypos;
	unsigned char	wins;
	unsigned char	faceleft;
	unsigned char	input;
	unsigned char	inputlast;
	unsigned char inputcpudelay;
};

struct GameData {
	unsigned char GameState;
	struct Fighter Player1;
	struct Fighter Player2;
};

static char jumpdistances[49] = { 
	8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 0, -1, -1, -2, -2, -2, -3, -3, -3, -4, -4, -4, -5, -5, -5, -5, -6, -6, -6, -6, -7, -7, -7, -7, -8
};

static char kickbackdistances[29] = { 
	5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 2, 2, 1, 1, 0, -1, -1, -2, -2, -3, -3, -4, -4, -4, -4, -5, -5, -5, -5
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

void fighterconfig(struct Fighter* fighter)
{
	fighter->state = FIGHTERSTATE_IDLE;
	fighter->statetime = 0;
	fighter->ypos = 0;
	fighter->input = 0;
	fighter->inputlast = 0;
	fighter->inputcpudelay = (unsigned char)(rand() % FIGHTER_CPU_MAXDELAY) + 1;
}

unsigned char isbuttonpressed(unsigned char previnput, unsigned char curinput, unsigned char buttoncheck)
{
	if( (curinput & buttoncheck) == 0 && (previnput & buttoncheck) != 0 )
	{
		return 1;
	}
	return 0;
}

void renderfightbackground()
{
	int idx;
	
	clrscr();
	bgcolor( COLOR_CYAN );
	for( idx = 0; idx < 440; ++idx )
	{
		SCREEN_MAP[560 + idx] = 0xa0; // menutitlechar[idx];
		COLOR_RAM[560 + idx] = COLOR_GREEN;
	}
}

void renderfighter(struct Fighter* fighter, unsigned char startsprite, unsigned char faceleft)
{
	int uploadaddr = 0x3C00 + (startsprite * 0x40);
	switch( fighter->state )
	{
		case FIGHTERSTATE_IDLE:
			setspritexy( startsprite, fighter->xpos - 24, FLOORY - 84 );
			setspritexy( startsprite + 1, fighter->xpos - 24, FLOORY - 42 );
			setspritexy( startsprite + 2, (startsprite * 50), 0 );
			setspritexy( startsprite + 3, (startsprite + 1) * 50, 0 );
			if( faceleft != fighter->faceleft || fighter->statetime == 0 )
			{
				fighter->faceleft = faceleft;
				memcpy((void*)(uploadaddr), (faceleft ? &SpriteLeftIdleT : &SpriteRightIdleT ), 64);
				memcpy((void*)(uploadaddr + 0x40), (faceleft ? &SpriteLeftIdleB : &SpriteRightIdleB ), 64);
			}
			break;
		case FIGHTERSTATE_JUMP:
		case FIGHTERSTATE_KICKBACK:
		case FIGHTERSTATE_HIT:
			setspritexy( startsprite, fighter->xpos - 24, FLOORY - fighter->ypos - 84 );
			setspritexy( startsprite + 1, fighter->xpos - 24, FLOORY - fighter->ypos - 42 );
			setspritexy( startsprite + 2, (startsprite * 50), 0 );
			setspritexy( startsprite + 3, (startsprite + 1) * 50, 0 );
			if( fighter->statetime == 0 )
			{
				memcpy((void*)(uploadaddr), (fighter->faceleft ? &SpriteLeftJumpT : &SpriteRightJumpT ), 64);
				memcpy((void*)(uploadaddr + 0x40), (fighter->faceleft ? &SpriteLeftJumpB : &SpriteRightJumpB ), 64);
			}
			break;
		case FIGHTERSTATE_KICK:
			setspritexy( startsprite, fighter->xpos - 48, FLOORY - fighter->ypos - 84 );
			setspritexy( startsprite + 1, fighter->xpos, FLOORY - fighter->ypos - 84 );
			setspritexy( startsprite + 2, fighter->xpos - 48, FLOORY - fighter->ypos - 42 );
			setspritexy( startsprite + 3, fighter->xpos, FLOORY - fighter->ypos - 42 );
			if( fighter->statetime == 0 )
			{
				memcpy((void*)(uploadaddr), (fighter->faceleft ? &SpriteLeftKickTL : &SpriteRightKickTL ), 64);
				memcpy((void*)(uploadaddr + 0x40), (fighter->faceleft ? &SpriteLeftKickTR : &SpriteRightKickTR ), 64);
				memcpy((void*)(uploadaddr + 0x80), (fighter->faceleft ? &SpriteLeftKickBL : &SpriteRightKickBL ), 64);
				memcpy((void*)(uploadaddr + 0xC0), (fighter->faceleft ? &SpriteLeftKickBR : &SpriteRightKickBR ), 64);
			}
			break;
		case FIGHTERSTATE_FLOORED:
			setspritexy( startsprite, fighter->xpos - 48, FLOORY - 42 );
			setspritexy( startsprite + 1, fighter->xpos, FLOORY - 42 );
			setspritexy( startsprite + 2, (startsprite * 50), 0 );
			setspritexy( startsprite + 3, (startsprite + 1) * 50, 0 );
			if( fighter->statetime == 0 )
			{
				memcpy((void*)(uploadaddr), (fighter->faceleft ? &SpriteLeftDeadL : &SpriteRightDeadL ), 64);
				memcpy((void*)(uploadaddr + 0x40), (fighter->faceleft ? &SpriteLeftDeadR : &SpriteRightDeadR ), 64);
			}
			break;
	}
}

void renderfight(struct GameData* data)
{
	unsigned char p1facingleft;
	
	p1facingleft = (data->Player1.xpos > data->Player2.xpos ? 1 : 0);
	
	renderfighter( &(data->Player1), 0, p1facingleft );
	renderfighter( &(data->Player2), 4, 1 - p1facingleft );
	
	// TODO: Render "Wins"
}

void renderinoverlay(struct GameData* data)
{
	unsigned char countdown = 3;
	
	while( countdown > 0 )
	{
		// TODO: Countdown intro
		waitframes( 2 );
		--countdown;
	}
	data->GameState = GAMESTATE_FIGHT;
}

void renderoutoverlay(struct GameData* data)
{
	// TODO: Wins
	waitframes( 10 );
	waitframesorbutton( 90 );
	
	fighterconfig( &data->Player1 );
	data->Player1.xpos = 70;
	data->Player1.faceleft = 1;
	fighterconfig( &data->Player2 );
	data->Player2.xpos = 260;
	data->Player2.faceleft = 0;
	data->GameState = GAMESTATE_ROUNDIN;
}

void rendermatchoverlay(struct GameData* data)
{
	unsigned char i;

	if( data->Player1.iscpu == 0 || data->Player2.iscpu == 0 )
	{
		// TODO: Draw complete over
		while( (JOY2[0] & JOY_FIRE) == JOY_FIRE && (JOY1[0] & JOY_FIRE) == JOY_FIRE )
		{
			// Do nothing until pressed fire
		}
	}
	
	for(i = 0; i < 8; i++ )
	{
		setspritexy( i, 0, 0 );
	}
	bgcolor( COLOR_BLUE );
	waitframes( 3 );
	bgcolor( COLOR_BLACK );
}

void cpucontrol(struct Fighter* fighter)
{
	
	if( fighter->inputcpudelay == 0 )
	{
		fighter->input = 0xff;
		switch( (rand() % 3) )
		{
			case 0:
				fighter->input -= JOY_UP;
				break;
			case 1:
				fighter->input -= JOY_FIRE;
				break;
			case 2:
				break;
		}
		fighter->inputcpudelay = (unsigned char)(rand() % FIGHTER_CPU_MAXDELAY) + 1;
	} else {
		--fighter->inputcpudelay;
	}
}

void setfighterstate(struct Fighter* fighter, unsigned char state)
{
	fighter->state = state;
	fighter->statetime = 0;
	
	if( state == FIGHTERSTATE_HIT && fighter->ypos == 0 )
	{
		fighter->state = FIGHTERSTATE_FLOORED;
	} else if ( state == FIGHTERSTATE_FLOORED || state == FIGHTERSTATE_IDLE ) {
		fighter->ypos = 0;
	}
}

void updatefighter(struct Fighter* fighter)
{
	++fighter->statetime;

	if( fighter->state == FIGHTERSTATE_IDLE )
	{
		fighter->statetime = 1;
		if( isbuttonpressed(fighter->inputlast, fighter->input, JOY_UP) )
		{
			setfighterstate( fighter, FIGHTERSTATE_JUMP );
		} else if( isbuttonpressed(fighter->inputlast, fighter->input, JOY_FIRE) ) {
			setfighterstate( fighter, FIGHTERSTATE_KICKBACK );
		}
	} else if( fighter->state == FIGHTERSTATE_JUMP || fighter->state == FIGHTERSTATE_KICKBACK ) {
		if( isbuttonpressed(fighter->inputlast, fighter->input, JOY_FIRE) ) {
			setfighterstate( fighter, FIGHTERSTATE_KICK );
		}
	}
	
	switch( fighter->state )
	{
		case FIGHTERSTATE_JUMP:
			fighter->ypos += jumpdistances[fighter->statetime];
			if( fighter->ypos <= 0 )
			{
				setfighterstate( fighter, FIGHTERSTATE_IDLE );
			}
			break;

		case FIGHTERSTATE_KICK:
			if( fighter->faceleft )
			{
				fighter->xpos -= 5;
			} else {
				fighter->xpos += 5;
			}
			if( fighter->ypos <= 5 )
			{
				setfighterstate( fighter, FIGHTERSTATE_IDLE );
			} else {
				fighter->ypos -= 5;
			}
			break;

		case FIGHTERSTATE_KICKBACK:
			fighter->ypos += kickbackdistances[fighter->statetime];
			if( fighter->faceleft )
			{
				fighter->xpos += 3;
			} else {
				fighter->xpos -= 3;
			}
			if( fighter->ypos <= 0 )
			{
				setfighterstate( fighter, FIGHTERSTATE_IDLE );
			}
			break;

		case FIGHTERSTATE_HIT:
			if( fighter->ypos > 4 )
			{
				fighter->ypos -= 4;
			} else {
				setfighterstate( fighter, FIGHTERSTATE_FLOORED );
			}
			break;
	}
	
	if( fighter->xpos < 24 )
	{
		fighter->xpos = 24;
	}
	
	if( fighter->xpos > 320 )
	{
		fighter->xpos = 320;
	}
	
}

unsigned char iscollide(struct Fighter* kickingplayer, unsigned char kicksprite, struct Fighter* targetplayer, unsigned char targetsprite)
{
	unsigned char footsprite;
	unsigned char result;
	
	footsprite = (kickingplayer->faceleft ? 2 : 3) + kicksprite;
	result = arespritesoverlapping( footsprite, targetsprite );
	if( targetplayer->state == FIGHTERSTATE_KICK )
	{
		result |= arespritesoverlapping( footsprite, targetsprite + 1 );
	}
	return result;
}

void updatefight(struct GameData* data)
{
	unsigned char collisions = 0;

	data->Player1.inputlast = data->Player1.input;
	data->Player2.inputlast = data->Player2.input;

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
	
	if( data->Player1.state == FIGHTERSTATE_KICK && data->Player2.state != FIGHTERSTATE_KICK )
	{
		if( iscollide( &data->Player1, 0, &data->Player2, 4 ) )
		{
			setfighterstate( &data->Player2, FIGHTERSTATE_HIT );
		}
	}
	if( data->Player1.state != FIGHTERSTATE_KICK && data->Player2.state == FIGHTERSTATE_KICK )
	{
		if( iscollide( &data->Player2, 4, &data->Player1, 0 ) )
		{
			setfighterstate( &data->Player1, FIGHTERSTATE_HIT );
		}
	}
	if( data->Player1.state == FIGHTERSTATE_KICK && data->Player2.state == FIGHTERSTATE_KICK )
	{
		collisions = iscollide( &data->Player1, 0, &data->Player2, 4 ) * 2;
		collisions += iscollide( &data->Player2, 4, &data->Player1, 0 );
		if( collisions >= 2 )
		{
			setfighterstate( &data->Player2, FIGHTERSTATE_HIT );
		}
		if( collisions == 1 || collisions == 3 )
		{
			setfighterstate( &data->Player1, FIGHTERSTATE_HIT );
		}
	}
	
	if( data->Player1.state == FIGHTERSTATE_FLOORED || data->Player2.state == FIGHTERSTATE_FLOORED )
	{
		if( (data->Player1.state == FIGHTERSTATE_FLOORED || data->Player1.state == FIGHTERSTATE_IDLE) && (data->Player2.state == FIGHTERSTATE_FLOORED || data->Player2.state == FIGHTERSTATE_IDLE) )
		{
			data->GameState = GAMESTATE_ROUNDOUT;
		} else {
			waitframes( 3 );
		}
	}
	
	if( data->Player1.iscpu && data->Player2.iscpu )
	{
		if( (JOY2[0] & JOY_FIRE) == 0 || (JOY1[0] & JOY_FIRE) == 0 )
		{
			data->GameState = GAMESTATE_MATCHOUT;
		}
	}
}

void fightstage(unsigned char P1CPU, unsigned char P2CPU)
{
	struct GameData data;

	fightspriteinit();
	renderfightbackground();

	data.GameState = GAMESTATE_ROUNDIN;
	
	fighterconfig( &data.Player1 );
	data.Player1.wins = 0;
	data.Player1.iscpu = P1CPU;
	data.Player1.xpos = 70;
	data.Player1.faceleft = 1;
	
	fighterconfig( &data.Player2 );
	data.Player2.wins = 0;
	data.Player2.iscpu = P2CPU;
	data.Player2.xpos = 260;
	data.Player2.faceleft = 0;
	
	while( data.GameState != GAMESTATE_MATCHOUT )
	{
		renderfight( &data );
		switch( data.GameState )
		{
			case GAMESTATE_ROUNDIN:
				renderinoverlay( &data );
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
