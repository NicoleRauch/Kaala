/////////////////////////////////////////////////////////////////////////
//		game.h


typedef unsigned char BYTE;
enum SIDE { TOP, BOT };
enum KIND { HUMAN, COMPUTER };
#define FALSE 0
#define TRUE 1
#define START_CONTENTS 6
#define MAX_MOVES 75


struct PLAYER 
{
	SIDE Side;
	KIND Kind;
	BYTE Strength;      
};

extern PLAYER  *Player, *CalcPlayer, PlayerInfo[2];		


class HOLE {
	protected:
		BYTE Contents;		//number of stones in the hole
		HOLE *Next, *Prev, *Oppos;
		SIDE Side;
	public:
		int SameSide( SIDE PlayerSide )
			{ return Side == PlayerSide;	}											
		BYTE GetContents(){ return Contents; }
		HOLE(){
			Contents = START_CONTENTS;
#ifdef __GNUG__			
			arcparams.lP = 1;
			arcparams.lQ = 1;
			arcparams.lR = 0;
			arcparams.lS = 0;
#endif			
		}
		virtual HOLE *Drop( BYTE *Stones, PLAYER *Player );
		virtual HOLE *Undo( BYTE *Stones, PLAYER *Player );
		BYTE Empty(){ BYTE i = Contents; Contents = 0; return i; }
		void Fill( BYTE Stones ){ Contents += Stones; }
		void TakeOut(BYTE Stones){ Contents -= Stones; }
		HOLE *GetNext(){ return Next; }
		HOLE *GetPrev(){ return Prev; }
		HOLE *GetOppos(){ return Oppos; }
		void SetValues( HOLE *PrevHole, HOLE *NextHole, 
							 HOLE *OpposHole, SIDE HoleSide );

#ifdef __GNUG__							 
	protected:							 
			ARCPARAMS arcparams;
			POINTL Center;						 
		public:						 
			VOID DrawOutline( HPS hps, PLAYER *Player );						 
			HOLE *Draw( HPS hps );
			VOID SetArcParams( ARCPARAMS p ){ arcparams = p; }					 
			VOID SetCenter( INT x, INT y ){ Center.x = x; Center.y = y; }
#endif				

};

class TOPHOME : public HOLE {
	public:
		TOPHOME(){
			Contents = 0;
#ifdef __GNUG__			
			arcparams.lQ = 2;
#endif			
		}
		virtual HOLE *Drop( BYTE *Stones, PLAYER *Player );
		virtual HOLE *Undo( BYTE *Stones, PLAYER *Player );
};

class BOTHOME : public HOLE {
	public:
		BOTHOME(){
			Contents = 0;
#ifdef __GNUG__			
			arcparams.lQ = 2;
#endif			
		}
		virtual HOLE *Drop( BYTE *Stones, PLAYER *Player );
		virtual HOLE *Undo( BYTE *Stones, PLAYER *Player );
};

class BOARD {
	protected:	// private??
		HOLE Top1, Top2, Top3, Top4, Top5, Top6, 
			  Bot1, Bot2, Bot3, Bot4, Bot5, Bot6;
		TOPHOME TopH;
		BOTHOME BotH;
		HOLE *TopStart, *BotStart;
		TOPHOME *TopHome;
		BOTHOME *BotHome;
	public:
		struct MOVES 
		{
			HOLE *Move;
			BYTE OppStones;
		};
		MOVES Moves[MAX_MOVES];
		char LastMove;
		HOLE *GetTopHome(){ return TopHome; }
		HOLE *GetBotHome(){ return BotHome; }
		HOLE *GetTopStart(){ return TopStart; }
		HOLE *GetBotStart(){ return BotStart; }
		HOLE *Number2Ptr(BYTE Number, SIDE Side);
		BYTE Ptr2Number(HOLE *Ptr, SIDE Side);
		BYTE HomeCont( SIDE Side ){ if( Side == TOP )
						return TopHome->GetContents();
					else	
						return BotHome->GetContents(); }
		BOARD();
		int Move( HOLE *MovePtr, PLAYER *Player ); 		// move-validity is already checked
		void OneUndo( PLAYER *Player );
		void Undo( PLAYER **Player );
		void Copy( BOARD &Src );
};



	
		
/////////////////////////////////////////////////////////////
// non-element function prototypes
		
void InitPlayerDefault();
void TogglePlayer( PLAYER **Player );
HOLE *GetComputerMove( int FirstMove, BOARD& Board );

/////////////////////////////////////////////////////////////
///////////globals//////////

extern BOARD Board, CalcBoard;

