//////////////////////////////////////////////////////////////////////
// Graphics routines needed by Kaala
#include <stdlib.h>

#define INCL_DOSPROCESS
#define INCL_WIN
#include <os2.h>
#include <stdlib.h>

#include "game.h"
#include "pmgame.h"
#include "kaala.h"


VOID GRAPHHOLE::DrawOutline( HPS hps )
{}

GRAPHHOLE *GRAPHHOLE::Draw( HPS hps )
{
	char cont[3];
	
	GpiSetColor( hps, CLR_BLACK );
	GpiSetArcParams( hps, &arcparams );
	GpiMove( hps, &Center );
	GpiFullArc( hps, DRO_OUTLINE, GraphBoard.SizeFactor );
	_itoa( Contents, cont, 10);
	GpiCharStringAt( hps, &Center, 2, cont );
	return (GRAPHHOLE *)Next;

}

void GRAPHHOLE::SetValues( GRAPHHOLE *PrevHole, GRAPHHOLE *NextHole, 
				   GRAPHHOLE *OpposHole, SIDE HoleSide )
{
	Next = NextHole;
	Prev = PrevHole;
	Oppos = OpposHole;
	Side = HoleSide;
}

GRAPHBOARD::GRAPHBOARD()
{
	BYTE i;

	Top1.SetValues( &BotH, &Top2, &Bot6, TOP );
	Top2.SetValues( &Top1, &Top3, &Bot5, TOP );
	Top3.SetValues( &Top2, &Top4, &Bot4, TOP );
	Top4.SetValues( &Top3, &Top5, &Bot3, TOP );
	Top5.SetValues( &Top4, &Top6, &Bot2, TOP );
	Top6.SetValues( &Top5, &TopH, &Bot1, TOP );
	TopH.SetValues( &Top6, &Bot1, &TopH, TOP );
	Bot1.SetValues( &TopH, &Bot2, &Top6, BOT );
	Bot2.SetValues( &Bot1, &Bot3, &Top5, BOT );
	Bot3.SetValues( &Bot2, &Bot4, &Top4, BOT );
	Bot4.SetValues( &Bot3, &Bot5, &Top3, BOT );
	Bot5.SetValues( &Bot4, &Bot6, &Top2, BOT );
	Bot6.SetValues( &Bot5, &BotH, &Top1, BOT );
	BotH.SetValues( &Bot6, &Top1, &BotH, BOT );	// set up the pointer list

	TopStart = &Top1;
	BotStart = &Bot1;
	TopHome = &TopH;
	BotHome = &BotH;

	for( i = 0; i < MAX_MOVES; i++ )
		Moves[i].OppStones = 0;
	LastMove = -1;		// index for Moves	
}


VOID GRAPHBOARD::Draw( HPS hps )
{
	GRAPHHOLE *ThisHole = (GRAPHHOLE *)GetTopStart();

	for( int i = 0; i < 14; i++ ){
		ThisHole = ThisHole->Draw( hps );
	}

}
	

VOID GRAPHBOARD::SetGraphics( INT Width, INT Height )
{
	INT dx = Width / 13;
	INT dy = Height / 5;
	INT diam = dx < dy ? dx : dy;

	SizeFactor = MAKEFIXED( diam / 2, 0);

	INT x = (Width - diam * 13 ) / 2 + diam * 5 / 4;
	INT y = (Height - diam * 5) / 2 + diam;

	TopH.SetCenter( x, y + diam * 3 / 2 );
	x += diam * 3 / 2;
	Bot1.SetCenter( x, y );
	Top6.SetCenter( x, y + 3 * diam );
	x += diam * 3/2;
	Bot2.SetCenter( x, y );
	Top5.SetCenter( x, y + 3 * diam );
	x += diam * 3/2;
	Bot3.SetCenter( x, y );
	Top4.SetCenter( x, y + 3 * diam );
	x += diam * 3/2;
	Bot4.SetCenter( x, y );
	Top3.SetCenter( x, y + 3 * diam );
	x += diam * 3/2;
	Bot5.SetCenter( x, y );
	Top2.SetCenter( x, y + 3 * diam );
	x += diam * 3/2;
	Bot6.SetCenter( x, y );
	Top1.SetCenter( x, y + 3 * diam );
	x += diam * 3/2;
	BotH.SetCenter( x, y + diam * 3 / 2);
}
	
/*
GRAPHBOARD::StoreWindowSize( INT Width, INT Height )
{
	WinWidth = Width;
	WinHeight = Height;
}
*/

