/////////////////////////////////////////////////////////////
//
//	simple text-mode test program

#include <stdio.h>
#include <iostream.h>
#include "game.h"

// globals
PLAYER  *Player, *CalcPlayer, PlayerInfo[2];		

void ShowBoard(BOARD Board)
{
	HOLE *BoardPtr;
	BYTE i;

	BoardPtr = Board.GetTopHome();
	for( i = 1; i <= 6; i++ ){
		BoardPtr = BoardPtr->GetPrev();
		printf( "%5d", BoardPtr->GetContents() );
	}
	printf("\n");
	printf(" %d                               %d \n",
			 Board.GetTopHome()->GetContents(),
			 Board.GetBotHome()->GetContents() );
	BoardPtr = Board.GetTopHome();	
	for( i = 1; i <= 6; i++ ){
		BoardPtr = BoardPtr->GetNext();
		printf( "%5d", BoardPtr->GetContents() );
	}
	printf("\n\nUNDO Buffer: %d\n", Board.LastMove);
	printf("\n\n\n");
}

int MakeMove(BOARD& Board)
{
	int Movenumber;
	HOLE *MovePtr, *TestPtr;
	HOLE *HomePtr = Player->Side == TOP ? Board.GetTopHome() : Board.GetBotHome();
	int Again = ( Board.LastMove == -1 || Board.Moves[Board.LastMove].Move != HomePtr);

	ShowBoard( Board );
	printf("\n");
	if (!Again) printf("2. ");
	printf("Zug fr Spieler ");
	Player->Side == TOP ? printf("Top: ") : printf("Bottom: ");
	scanf("%d", &Movenumber );
	if( Movenumber < 0 || Movenumber > 6 ) return FALSE;
	if( !Movenumber )
	{
		if( Board.LastMove != -1)
		{  //ensure that a move was made
			Board.Undo( &Player );
		} else
		{
			printf(" Ein UNDO ist nicht m”glich, da noch kein Zug erfolgte!\n");
		}
		TogglePlayer( &Player );
		return TRUE;
	}
	MovePtr = Board.Number2Ptr( Movenumber, Player->Side );
	if( !MovePtr->GetContents() )
	{	// empty hole
		TestPtr = (Player->Side == TOP ?
					  Board.GetTopStart() : Board.GetBotStart() );
		while( !TestPtr->GetContents() && TestPtr != HomePtr )
			TestPtr = TestPtr->GetNext();
		if( TestPtr == HomePtr )
		{
			printf("Fr Sie ist kein Zug mehr m”glich. Daher hat der andere gewonnen\n");
			return FALSE;
		} else
		{
			printf("Dieser Zug ist ungltig!\n");
			TogglePlayer( &Player );
			return TRUE;
		}
	}

	if( Board.Move( MovePtr, Player ) && Board.HomeCont( Player->Side ) <= 36 &&	
		 Again )
	{
		MakeMove( Board );	// move once again
	}
	return TRUE;
}

	
int main()
{
	int quit = 0;
	int CompDepth;
	HOLE *CompMove;
	InitPlayerDefault(Board);
	HOLE *HomePtr = Player->Side == TOP ? Board.GetTopHome() : Board.GetBotHome();
	int again = ( Board.LastMove == -1 || Board.Moves[Board.LastMove].Move != HomePtr);

	printf("Welche Spielst„rke fr den Computer? ");
	scanf("%d", &CompDepth );
	printf("\n\n");
	PlayerInfo[1].Strength = CompDepth;
	
	while(!quit)
	{
		if(Player->Kind == HUMAN)
		{
			quit = !MakeMove(Board);
		}
		else
		{	
			if( !quit )
			{
				ShowBoard( Board );
				CompMove = GetComputerMove( TRUE );
				if( CompMove == NULL )
				{
					printf("Kein Zug mehr m”glich!\n");
					quit = 1;
				}
				else
					printf("Computerzug: %d\n", Board.Ptr2Number( CompMove, Player->Side ) );
					again = Board.Move( CompMove, Player );
				if( Board.HomeCont( Player->Side) > 36) quit = 1;
				if( again && !quit )
				{
					ShowBoard( Board ); 
					CompMove = GetComputerMove( FALSE );
					if( CompMove == NULL )
					{
						printf("Kein Zug mehr m”glich!\n");
						quit = 1;
					}
					else
						printf("Computerzug: %d\n", Board.Ptr2Number( CompMove, Player->Side ) );
						Board.Move( CompMove, Player );
					if(Board.HomeCont( Player->Side) > 36) quit = 1;
				}
			}
		}
		TogglePlayer( &Player );
		HomePtr = HomePtr->GetOppos();
	}	
	printf("Zahl der ben”tigten Zge: %d\n", Board.LastMove + 1 );

	TogglePlayer( &Player );
	if( Board.HomeCont( Player->Side ) > 36)
	{
		ShowBoard(Board);
		printf("Herzlichen Glckwunsch! Spieler ");
		Player->Side == TOP ? printf("Top ") : printf("Bottom ");
		printf("hat gewonnen!");
	}
	return 0;
}
	
