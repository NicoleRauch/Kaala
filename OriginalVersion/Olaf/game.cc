//

#include <stdio.h>
#include "compile.h"

#define INITIALIZE
#include "game.h"

#define DEFAULT_STRENGTH	4

HOLE *HOLE::Drop(BYTE& Stones )
{
	Contents++;
	Stones--;
	return Next;
}

HOLE *HOLE::Undo(BYTE& Stones )
{
	Contents--;
	Stones++;
	return Prev;
}

VOID HOLE::SetValues( HOLE *PrevHole, HOLE *NextHole, 
				   HOLE *OpposHole, SIDE HoleSide )
{
	Next = NextHole;
	Prev = PrevHole;
	Oppos = OpposHole;
	Side = HoleSide;
}

HOLE *TOPHOME::Drop(BYTE& Stones )
{
	if( Player->Side  == TOP ){
		Contents++;
		Stones--;
	}
	return Next;
}

HOLE *TOPHOME::Undo(BYTE& Stones )
{
	if( Player->Side  == TOP ){
		Contents--;
		Stones++;
	}
	return Prev;
}

HOLE *BOTHOME::Drop(BYTE& Stones )
{
	if( Player->Side  == BOT ){
		Contents++;
		Stones--;
	}
	return Next;
}

HOLE *BOTHOME::Undo(BYTE& Stones )
{
	if( Player->Side  == BOT ){
		Contents--;
		Stones++;
	}
	return Prev;
}

BOARD::BOARD()
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
	BotH.SetValues( &Bot6, &Top1, &BotH, BOT );	// set up the pointer-list

	TopStart = &Top1;
	BotStart = &Bot1;
	TopHome = &TopH;
	BotHome = &BotH;

	for( i = 0; i < 50; i++ )
		Moves[i].OppStones = 0;
	LastMove = -1;		// index for Moves	
}

int BOARD::Move( BYTE MovePos )
{
	HOLE *MovePtr = (Player->Side == TOP ? TopStart : BotStart);
//	HOLE *HomePtr = Player->Side == TOP ? TopHome : BotHome;
	BYTE Stones;

	LastMove++;
	for( ; MovePos > 1; MovePos-- )
		MovePtr = MovePtr->GetNext();
	Stones = MovePtr->Empty();
	MovePtr = MovePtr->GetNext();
	while( Stones )
		MovePtr = MovePtr->Drop( Stones );
	MovePtr = MovePtr->GetPrev();
	Moves[LastMove].Move = MovePtr;
	if( MovePtr == (Player->Home) ) return TRUE;
	if( MovePtr->GetContents() == 1 && MovePtr->SameSide(Player->Side) ){
		Player->Home->Fill( MovePtr->Empty() );
		MovePtr = MovePtr->GetOppos();
		Player->Home->Fill( (Moves[LastMove].OppStones = MovePtr->Empty()) );	//??
	}
	return FALSE;
}

VOID BOARD::OneUndo( BYTE& LastMove )
{
	HOLE *Move = Moves[LastMove].Move;
//	HOLE *HomePtr = Player->Side == TOP ? TopHome : BotHome;
	BYTE Stones = 0;
	
	if( !Move->GetContents() )
	{
		Player->Home->TakeOut(Moves[LastMove].OppStones + 1);
		Move = Move->GetOppos();
		Move->Fill( Moves[LastMove].OppStones );
		Move = Move->GetOppos();
		Move->Fill( 1 );
	}
	while( Move->GetContents() )
		Move = Move->Undo( Stones );
	Move->Fill( Stones );
	Moves[LastMove].OppStones = 0;
	LastMove--;
}

VOID BOARD::Undo( BYTE& LastMove )
{
	OneUndo( LastMove );
	if( Moves[LastMove].Move == Player->Home )
		OneUndo( LastMove );
	TogglePlayer();
}	
	
	
	





//////////////////////////////////////////////////////////////////////
//	non-element functions

PUBLIC VOID InitPlayerDefault(BOARD Board)
{
	Player = PlayerInfo;
	PlayerInfo[0].Side = TOP;		// FIXED!!
	PlayerInfo[0].Home = Board.TopHome;
	PlayerInfo[0].Kind = HUMAN;
	PlayerInfo[0].Strength = DEFAULT_STRENGTH;
	PlayerInfo[1].Side = BOT;		// FIXED!!
	PlayerInfo[1].Home = Board.BotHome;
	PlayerInfo[1].Kind = COMPUTER;
	PlayerInfo[1].Strength = DEFAULT_STRENGTH;
}

PUBLIC VOID TogglePlayer()	// inline
{
	if( Player->Side == TOP ) Player++; // = &PlayerInfo[1];
	else Player--; // = PlayerInfo;
}

PUBLIC VOID ToggleCalcPlayer()		// inline
{
	if( CalcPlayer->Side == TOP ) CalcPlayer++;
	else CalcPlayer--;
}

