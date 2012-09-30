//////////////////////////////////////////////////////////////
//
//	simple text-mode test program

#include <stdio.h>
#include <iostream.h>
#include "compile.h"
#include "game.h"

VOID ShowBoard(BOARD Board)
{
	HOLE *BoardPtr;
	BYTE i;

	BoardPtr = Board.TopHome;
	printf( "   " );
	for( i = 1; i <= 6; i++ ){
		BoardPtr = BoardPtr->GetPrev();
		printf( " %d   ", BoardPtr->GetContents() );
	}
	printf("\n");
	printf(" %d                               %d \n",
			 Board.TopHome->GetContents(),
			 Board.BotHome->GetContents() );
	BoardPtr = Board.TopHome;	
	printf( "   " );
	for( i = 1; i <= 6; i++ ){
		BoardPtr = BoardPtr->GetNext();
		printf( " %d   ", BoardPtr->GetContents() );
	}
	printf("\n");
}

int MakeMove(BOARD& Board)
{
	int Movenumber;
	char in[10];
	printf("\nZug fŸr Spieler ");
	Player->Side == TOP ? printf("Top: ") : printf("Bottom: ");
	scanf("%d", &Movenumber );
	if( Movenumber < 1 || Movenumber > 6 ) return FALSE;
	if( Board.Move(Movenumber) && Player->Home->GetContents() <= 36 ){
		ShowBoard(Board);
		printf("\nSie sind nochmal dran! Welcher Zug?: ");
		scanf( "%d", &Movenumber);
		if( Movenumber < 1 || Movenumber > 6 ) return FALSE;
		Board.Move(Movenumber);
	}
	return TRUE;
}
	
int main()
{
	BOARD Board;
	InitPlayerDefault(Board);
	ShowBoard(Board);
	int Wait;
	while ( MakeMove(Board) && Player->Home->GetContents() <= 36){
		TogglePlayer();
		ShowBoard(Board);
	}
	if(Player->Home->GetContents() > 36){
		ShowBoard(Board);
		printf("Herzlichen GlŸckwunsch! Spieler ");
		Player->Side == TOP ? printf("Top ") : printf("Bottom ");
		printf("hat gewonnen!");
	}
	scanf( "%d", &Wait );
	return 0;
}
	
