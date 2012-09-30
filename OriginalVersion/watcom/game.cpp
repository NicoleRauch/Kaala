//////////////////////////////////////////////////////////////
//	game.cc


#ifdef __GNUG__
#define INCL_WIN
#include <os2.h>
#endif


#include <stdio.h>
#include "game.h"
#define DEFAULT_STRENGTH	4

BYTE maxdepth;
const int inf = 30000;
BYTE depth;
BOARD Board, CalcBoard;

//////////////////////////////////////////////////////////////
//	prototypes

int FindMax(HOLE *MoveHole, const int alpha, const int beta,
			 const int FirstMove);
			 
int FindMin(HOLE *MoveHole, const int alpha, const int beta,
			 const int FirstMove);

//////////// globals

PLAYER  *Player, *CalcPlayer, PlayerInfo[2];		
			 

//////////////////////////////////////////////////////////////////
//	member functions

HOLE *HOLE::Drop(BYTE *Stones, PLAYER *Player )
{
	Contents++;
	(*Stones)--;
	return Next;
}

HOLE *HOLE::Undo(BYTE *Stones, PLAYER *Player )
{
	Contents--;
	(*Stones)++;
	return Prev;
}

void HOLE::SetValues( HOLE *PrevHole, HOLE *NextHole, 
				   HOLE *OpposHole, SIDE HoleSide )
{
	Next = NextHole;
	Prev = PrevHole;
	Oppos = OpposHole;
	Side = HoleSide;
}

HOLE *TOPHOME::Drop(BYTE *Stones, PLAYER *Player )
{
	if( Player->Side  == TOP )
	{
		Contents++;
		(*Stones)--;
	}
	return Next;
}

HOLE *TOPHOME::Undo(BYTE *Stones, PLAYER *Player )
{
	if( Player->Side  == TOP )
	{
		Contents--;
		(*Stones)++;
	}
	return Prev;
}

HOLE *BOTHOME::Drop(BYTE *Stones, PLAYER *Player )
{
	if( Player->Side  == BOT )
	{
		Contents++;
		(*Stones)--;
	}
	return Next;
}

HOLE *BOTHOME::Undo(BYTE *Stones, PLAYER *Player )
{
	if( Player->Side  == BOT ){
		Contents--;
		(*Stones)++;
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
	BotH.SetValues( &Bot6, &Top1, &BotH, BOT );	// set up the pointer list

	TopStart = &Top1;
	BotStart = &Bot1;
	TopHome = &TopH;
	BotHome = &BotH;

	for( i = 0; i < MAX_MOVES; i++ )
		Moves[i].OppStones = 0;
	LastMove = -1;		// index for Moves	
}

HOLE *BOARD::Number2Ptr(BYTE Number, SIDE Side)
{
	HOLE *MovePtr = (Side == TOP ? (HOLE*)TopStart : (HOLE*)BotStart);

	for( ; Number > 1; Number-- )
		MovePtr = MovePtr->GetNext();
	return MovePtr;
}

BYTE BOARD::Ptr2Number(HOLE *Ptr, SIDE Side)
{
	BYTE Number = 1;
	HOLE *MovePtr = (Side == TOP ? (HOLE*)TopStart : (HOLE*)BotStart);

	while( MovePtr != Ptr )
	{
		Number++;
		MovePtr = MovePtr->GetNext();
	}
	return Number;
}

int BOARD::Move(HOLE *MovePtr, PLAYER *Player )
{
//	HOLE *MovePtr = (Player->Side == TOP ? TopStart : BotStart);
	HOLE *HomePtr = (Player->Side == TOP ? (HOLE*)TopHome : (HOLE*)BotHome);
	BYTE Stones;

	LastMove++;
	Stones = MovePtr->Empty();
	MovePtr = MovePtr->GetNext();
	while( Stones )
		MovePtr = MovePtr->Drop( &Stones, Player );
	MovePtr = MovePtr->GetPrev();
	Moves[LastMove].Move = MovePtr;
	if( MovePtr == HomePtr ) return TRUE;
	if( MovePtr->GetContents() == 1 && MovePtr->SameSide(Player->Side) &&
		 MovePtr->GetOppos()->GetContents() ){
		HomePtr->Fill( MovePtr->Empty() );
		MovePtr = MovePtr->GetOppos();
		HomePtr->Fill( (Moves[LastMove].OppStones = MovePtr->Empty()) );	//??
	}
	return FALSE;
}

void BOARD::OneUndo( PLAYER *Player )
{
	HOLE *Move = Moves[LastMove].Move;
	HOLE *HomePtr = Player->Side == TOP ? (HOLE*)TopHome : (HOLE*)BotHome;
	BYTE Stones = 0;
	
	if( !Move->GetContents() && Moves[LastMove].OppStones )
	{
		HomePtr->TakeOut(Moves[LastMove].OppStones + 1);
		Move = Move->GetOppos();
		Move->Fill( Moves[LastMove].OppStones );
		Move = Move->GetOppos();
		Move->Fill( 1 );
		Moves[LastMove].OppStones = 0;
	}
	while( Move->GetContents() )
		Move = Move->Undo( &Stones, Player );
	Move->Fill( Stones );
	LastMove--;
}

void BOARD::Undo( PLAYER **Player )
{
	HOLE *HomePtr = ((*Player)->Side == TOP ? (HOLE*)TopHome : (HOLE*)BotHome);
	if( Moves[LastMove].Move == HomePtr ){	// if undo is called 
											// after the first
											// of two moves
		OneUndo( *Player );	// move is taken back, same player's turn
	} else {
		TogglePlayer( Player );
		OneUndo( *Player );
		if( (*Player)->Kind == COMPUTER ){
			HomePtr = ((*Player)->Side == TOP ? (HOLE*)TopHome : (HOLE*)BotHome);
			if( Moves[LastMove].Move == HomePtr )	// take back Computer's second move
				OneUndo( *Player );
			Undo( Player );	// Undo once again for the Human
		}	
	}
}	
	
void BOARD::Copy( BOARD &Src )
{
	HOLE *SrcTemp = Src.GetTopStart();
	HOLE *DestTemp = TopStart;

	do{
		DestTemp->Empty();
		DestTemp->Fill( SrcTemp->GetContents() );
		DestTemp = DestTemp->GetNext();
		SrcTemp = SrcTemp->GetNext();
	} while( SrcTemp != Src.GetTopStart() );
}

	
	
	
	

//////////////////////////////////////////////////////////////////////
//	non-element functions



void InitPlayerDefault()
{
	Player = PlayerInfo;
	PlayerInfo[0].Side = TOP;					// FIXED!!
	PlayerInfo[0].Kind = HUMAN;
	PlayerInfo[0].Strength = DEFAULT_STRENGTH;
	PlayerInfo[1].Side = BOT;					// FIXED!!
	PlayerInfo[1].Kind = COMPUTER;
	PlayerInfo[1].Strength = DEFAULT_STRENGTH;
}

void TogglePlayer( PLAYER **Player )	// inline
{
	if( (*Player)->Side == TOP ) (*Player)++; // = &PlayerInfo[1];
	else (*Player)--; // = PlayerInfo;
}


inline int ValFunc()
{
	return CalcBoard.HomeCont( CalcPlayer->Side ) - 
		   CalcBoard.HomeCont( (SIDE)!CalcPlayer->Side );
}


int FindMin(HOLE *MoveHole, const int alpha, const int beta,
			 const int FirstMove)
{
	int again;
	int MinMax;
	int New;
	HOLE *Testhole;
	
	again = CalcBoard.Move( MoveHole, CalcPlayer );
	if (CalcBoard.HomeCont( CalcPlayer->Side ) > 36 )
	{
		MinMax = -inf + depth;
	}
	else
	{
		depth++;
		if ( depth == maxdepth )
		{
			MinMax = -ValFunc();
		}
		else
		{
			if ( again && FirstMove )
			{
				MinMax = beta;
				for ( Testhole = (CalcPlayer->Side == TOP ? 
		 			 CalcBoard.GetTopStart() : CalcBoard.GetBotStart());
		 			 
		 			 MinMax > alpha &&
	 	 			 Testhole != (CalcPlayer->Side == TOP ? 
		 			 CalcBoard.GetTopHome() : CalcBoard.GetBotHome());
		  
		  			 Testhole = Testhole->GetNext() )
				{
					if (Testhole->GetContents())
					{
						New = FindMin(Testhole, alpha, MinMax, FALSE );
						if (New < MinMax)
							MinMax = New;
					}
				}
			}
			else
			{
				TogglePlayer( &CalcPlayer );
				MinMax = alpha;
				for ( Testhole = (CalcPlayer->Side == TOP ? 
		 			 CalcBoard.GetTopStart() : CalcBoard.GetBotStart());
		 			 
		 			 MinMax < beta &&
	 	 			 Testhole != (CalcPlayer->Side == TOP ? 
		 			 CalcBoard.GetTopHome() : CalcBoard.GetBotHome());
		  
		  			 Testhole = Testhole->GetNext() )
				{
					if (Testhole->GetContents())
					{
						New = FindMax(Testhole, MinMax, beta, TRUE );
						if (New > MinMax)
							MinMax = New;
					}
				}
				TogglePlayer( &CalcPlayer );
			}
		}	
		depth--;
	}
	CalcBoard.OneUndo( CalcPlayer );
	return MinMax;
}	// FindMin



int FindMax(HOLE *MoveHole, const int alpha, const int beta,
			 const int FirstMove)
{
	int again;
	int MinMax;
	int New;
	HOLE *Testhole;
	
	again = CalcBoard.Move( MoveHole, CalcPlayer );
	if (CalcBoard.HomeCont( CalcPlayer->Side ) > 36 )
	{
		MinMax = inf - depth;
	}
	else
	{
		depth++;
		if ( depth == maxdepth )
		{
			MinMax = ValFunc();
		}
		else
		{
			if ( again && FirstMove )
			{
				MinMax = alpha;
				for ( Testhole = (CalcPlayer->Side == TOP ? 
		 			 CalcBoard.GetTopStart() : CalcBoard.GetBotStart());
		 			 
		 			 MinMax < beta &&
	 	 			 Testhole != (CalcPlayer->Side == TOP ? 
		 			 CalcBoard.GetTopHome() : CalcBoard.GetBotHome());
		  
		  			 Testhole = Testhole->GetNext() )
				{
					if (Testhole->GetContents())
					{
						New = FindMax(Testhole, MinMax, beta, FALSE );
						if (New > MinMax)
							MinMax = New;
					}
				}
			}
			else
			{
				TogglePlayer( &CalcPlayer );
				MinMax = beta;
				for ( Testhole = (CalcPlayer->Side == TOP ? 
		 			 CalcBoard.GetTopStart() : CalcBoard.GetBotStart());
		 			 
		 			 MinMax > alpha &&
	 	 			 Testhole != (CalcPlayer->Side == TOP ? 
		 			 CalcBoard.GetTopHome() : CalcBoard.GetBotHome());
		  
		  			 Testhole = Testhole->GetNext() )
				{
					if (Testhole->GetContents())
					{
						New = FindMin(Testhole, alpha, MinMax, TRUE );
						if (New < MinMax)
							MinMax = New;
					}
				}
				TogglePlayer( &CalcPlayer );
			}
		}	
		depth--;
	}
	CalcBoard.OneUndo( CalcPlayer );
	return MinMax;
}	// FindMax
	
			 
			 
			 

HOLE *GetComputerMove( int FirstMove, BOARD& Board )
{	
	int New;
	int MinMax = -inf;
	HOLE *Testhole, *Besthole = NULL;
	
	CalcBoard.Copy( Board );
	CalcPlayer = Player;
	maxdepth = CalcPlayer->Strength;
	depth = 0;
	for ( Testhole = (CalcPlayer->Side == TOP ? 
		  CalcBoard.GetTopStart() : CalcBoard.GetBotStart());
		  
	 	  Testhole != (CalcPlayer->Side == TOP ? 
		  CalcBoard.GetTopHome() : CalcBoard.GetBotHome());
		  
		  Testhole = Testhole->GetNext() )
	{
		if (Testhole->GetContents())
		{
			New = FindMax( Testhole, MinMax, inf, FirstMove );
			if (New >= MinMax)
			{
				MinMax = New;
				Besthole = Testhole;
			}
		}
	}
	if( Besthole == NULL ) return Besthole;
	else
		return Board.Number2Ptr( CalcBoard.Ptr2Number(Besthole, CalcPlayer->Side),
			 Player->Side );

}  // GetComputerMove
