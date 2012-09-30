/////////////////////////////////////////////////////////////
//
//

#ifdef INITIALIZE
#undef PUBLIC
#define PUBLIC
#endif


typedef unsigned char BYTE;
typedef void VOID;
enum SIDE { TOP, BOT };
enum KIND { HUMAN, COMPUTER };
#define FALSE 0
#define TRUE 1

class HOLE {
	protected:
		BYTE Contents;		//number of stones in the hole
		HOLE *Next, *Prev, *Oppos;
		SIDE Side;
	public:
		int SameSide( SIDE PlayerSide )
		{ if( Side == PlayerSide) return TRUE;
		  else return FALSE;	}											
		BYTE GetContents(){ return Contents; }
		HOLE(){ Contents = 6; }
		virtual HOLE *Drop( BYTE& Stones );
		virtual HOLE *Undo( BYTE& Stones );
		BYTE Empty(){ BYTE i = Contents; Contents = 0; return i; }
		VOID Fill( BYTE Stones ){ Contents += Stones; }
		VOID TakeOut(BYTE Stones){ Contents -= Stones; }
		HOLE *GetNext(){ return Next; }
		HOLE *GetPrev(){ return Prev; }
		HOLE *GetOppos(){ return Oppos; }
		VOID SetValues( HOLE *PrevHole, HOLE *NextHole, 
							 HOLE *OpposHole, SIDE HoleSide );
};

class TOPHOME : public HOLE {
	public:
		TOPHOME(){ Contents = 0; }
		virtual HOLE *Drop( BYTE& Stones );
		virtual HOLE *Undo( BYTE& Stones );
};

class BOTHOME : public HOLE {
	public:
		BOTHOME(){ Contents = 0; }
		virtual HOLE *Drop( BYTE& Stones );
		virtual HOLE *Undo( BYTE& Stones );
};

class BOARD {
	protected:	// private??
		HOLE Top1, Top2, Top3, Top4, Top5, Top6, 
			  Bot1, Bot2, Bot3, Bot4, Bot5, Bot6;
		TOPHOME TopH;
		BOTHOME BotH;
	public:
		struct MOVES {
			HOLE *Move;
			BYTE OppStones;
		};
		MOVES Moves[50];
		char LastMove;
		HOLE *TopStart, *BotStart;
		TOPHOME *TopHome;
		BOTHOME *BotHome;
		BYTE TopHomeCont(){ return TopHome->GetContents(); }
		BYTE BotHomeCont(){ return BotHome->GetContents(); }
		BOARD();
		int Move( BYTE MovePos ); 	// move-validity is already checked
		VOID OneUndo( BYTE& LastMove );
		VOID Undo( BYTE& LastMove );
};



	
struct PLAYER {
	SIDE Side;
	HOLE *Home;
	KIND Kind;
	BYTE Strength;
};

PUBLIC PLAYER  *Player, *CalcPlayer, PlayerInfo[2];		
		
/////////////////////////////////////////////////////////////
// non-element function prototypes
		
PUBLIC VOID InitPlayerDefault(BOARD Board);
PUBLIC VOID TogglePlayer();
PUBLIC VOID ToggleCalcPlayer();

