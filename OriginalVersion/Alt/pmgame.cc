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


VOID HOLE::DrawOutline( HPS hps, PLAYER *Player )
{
	GpiSetArcParams( hps, &arcparams );
	GpiMove( hps, &Center );
	GpiSetLineWidth( hps, LINEWIDTH_THICK );
	GpiSetColor( hps, Player->Side == TOP ? CLR_RED : CLR_BLUE );
	GpiFullArc( hps, DRO_OUTLINE, GraphBoard.SizeFactor );
	DosSleep( 50 );
	GpiSetColor( hps, CLR_WHITE );
	GpiFullArc( hps, DRO_OUTLINE, GraphBoard.SizeFactor );

	GpiSetLineWidth( hps, LINEWIDTH_NORMAL );
	GpiSetColor( hps, CLR_BLACK );
	GpiFullArc( hps, DRO_OUTLINE, GraphBoard.SizeFactor );
	

}

HOLE *HOLE::Draw( HPS hps )
{
	char cont[3];
	
	GpiSetArcParams( hps, &arcparams );
	GpiMove( hps, &Center );
	GpiSetColor( hps, CLR_WHITE );
	GpiFullArc( hps, DRO_FILL, GraphBoard.SizeFactor );
	GpiSetColor( hps, CLR_BLACK );
	GpiFullArc( hps, DRO_OUTLINE, GraphBoard.SizeFactor );
	_itoa( Contents, cont, 10);
	GpiCharStringAt( hps, &Center, 2, cont );
	return Next;

}


GRAPHBOARD::GRAPHBOARD() : BOARD()
{}


VOID GRAPHBOARD::Draw( HPS hps )
{
	HOLE *ThisHole = GetTopStart();

	for( int i = 0; i < 14; i++ ){
		ThisHole = ThisHole->Draw( hps );
	}

}
	

VOID GRAPHBOARD::SetGraphics( INT Width, INT Height )
{
	INT dx = Width / 13;
	INT dy = Height / 5;
	Diameter = dx < dy ? dx : dy;

	SizeFactor = MAKEFIXED( Diameter / 2, 0);
	xOffset = (Width - Diameter * 13 ) / 2;
	yOffset = (Height - Diameter * 5) / 2;

	INT x = xOffset + Diameter * 5 / 4;
	INT y = yOffset + Diameter;

	TopH.SetCenter( x, y + Diameter * 3 / 2 );
	x += Diameter * 3 / 2;
	Bot1.SetCenter( x, y );
	Top6.SetCenter( x, y + 3 * Diameter );
	x += Diameter * 3/2;
	Bot2.SetCenter( x, y );
	Top5.SetCenter( x, y + 3 * Diameter );
	x += Diameter * 3/2;
	Bot3.SetCenter( x, y );
	Top4.SetCenter( x, y + 3 * Diameter );
	x += Diameter * 3/2;
	Bot4.SetCenter( x, y );
	Top3.SetCenter( x, y + 3 * Diameter );
	x += Diameter * 3/2;
	Bot5.SetCenter( x, y );
	Top2.SetCenter( x, y + 3 * Diameter );
	x += Diameter * 3/2;
	Bot6.SetCenter( x, y );
	Top1.SetCenter( x, y + 3 * Diameter );
	x += Diameter * 3/2;
	BotH.SetCenter( x, y + Diameter * 3 / 2);
}
	
/*
GRAPHBOARD::StoreWindowSize( INT Width, INT Height )
{
	WinWidth = Width;
	WinHeight = Height;
}
*/

HOLE *GRAPHBOARD::GetMovingHole( POINTL ptl, PLAYER *Player )
{
	INT Number = ptl.x - xOffset, ytest = ptl.y - yOffset;
	if( ytest < Diameter / 4 || ytest > 19 * Diameter/4 ) return NULL;
	if( Number < Diameter * 2 || Number > Diameter * 11 )
		return NULL;
	if( Player->Side == BOT ){
		if( ytest > Diameter * 7 / 4) return NULL;
	}
	else{	// Player->Side == TOP
		if( ytest < Diameter * 13 / 4 ) return NULL;
	}
		
	// alle falschen Tastenklicks sind ausgeschlossen, jetzt gilt es
	// das richtige Hole zu finden	

	Number -= 2 * Diameter;
	Number /= Diameter * 3 / 2;
	Number++;
	if( Player->Side == TOP ) Number = 7 - Number;
			// hier wird andersrum numeriert
	return Number2Ptr( Number, Player->Side );		
}

// this is the same function as in BOARD, except some graphics features
// are added

INT GRAPHBOARD::Move( HWND hwnd, HOLE *MovePtr, PLAYER *Player )
{
	HOLE *HomePtr = (Player->Side == TOP ? (HOLE*)TopHome : (HOLE*)BotHome);
	BYTE Stones;
	HPS hps = WinGetPS( hwnd );
	BOOL RetValue;

	LastMove++;
	MovePtr->DrawOutline( hps, Player );
	Stones = MovePtr->Empty();
	MovePtr = MovePtr->GetNext();
	MovePtr->GetPrev()->Draw( hps );
	while( Stones ){
		MovePtr->DrawOutline( hps, Player );
		MovePtr = MovePtr->Drop( &Stones, Player );
		MovePtr->GetPrev()->Draw( hps );
	}
	MovePtr = MovePtr->GetPrev();
	Moves[LastMove].Move = MovePtr;
	if( MovePtr == HomePtr ) RetValue = TRUE;
	else {
		if( MovePtr->GetContents() == 1 && MovePtr->SameSide(Player->Side) &&
			 MovePtr->GetOppos()->GetContents() ){
			HomePtr->Fill( MovePtr->Empty() );
			MovePtr->Draw( hps );
			MovePtr = MovePtr->GetOppos();
		   HomePtr->Fill( (Moves[LastMove].OppStones = MovePtr->Empty()) );
			MovePtr->Draw( hps );
			HomePtr->Draw( hps );
		}
		RetValue = FALSE;
	}
	WinReleasePS( hps );
	return RetValue;
}
