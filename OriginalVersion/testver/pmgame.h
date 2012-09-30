/////////////////////////////////////////////////////////////////////

class GRAPHHOLE : public HOLE {
	protected:
		GRAPHHOLE *Next, *Prev, *Oppos;
		ARCPARAMS arcparams;
		POINTL Center;
	public:
		VOID DrawOutline( HPS hps );
		GRAPHHOLE *Draw( HPS hps );
		VOID SetArcParams( ARCPARAMS p ){ arcparams = p; }
		VOID SetCenter( INT x, INT y ){ Center.x = x; Center.y = y; }
		GRAPHHOLE() : HOLE() {
			arcparams.lP = 1;
			arcparams.lQ = 1;
			arcparams.lR = 0;
			arcparams.lS = 0;
		}
		void SetValues( GRAPHHOLE *PrevHole, GRAPHHOLE *NextHole, 
							 GRAPHHOLE *OpposHole, SIDE HoleSide );

};

class GRAPHTOPHOME : public TOPHOME, public GRAPHHOLE {
	public:
		GRAPHTOPHOME() : TOPHOME(), GRAPHHOLE() {
			arcparams.lQ = 2;
		}
		VOID SetValues( GRAPHHOLE *PrevHole, GRAPHHOLE *NextHole, 
							 GRAPHHOLE *OpposHole, SIDE HoleSide )
		{
		GRAPHHOLE::SetValues( PrevHole, NextHole, OpposHole, HoleSide );
		}
					 

};	

class GRAPHBOTHOME : public BOTHOME, public GRAPHHOLE {
	public:
		GRAPHBOTHOME() : BOTHOME(), GRAPHHOLE() {
			arcparams.lQ = 2;
		}
		VOID SetValues( GRAPHHOLE *PrevHole, GRAPHHOLE *NextHole, 
							 GRAPHHOLE *OpposHole, SIDE HoleSide )
		{
		GRAPHHOLE::SetValues( PrevHole, NextHole, OpposHole, HoleSide );
		}
					 
};


class GRAPHBOARD : public BOARD {
	protected:
		// create new holes with graphics abilities
		GRAPHHOLE Top1, Top2, Top3, Top4, Top5, Top6,
		     Bot1, Bot2, Bot3, Bot4, Bot5, Bot6;
      GRAPHTOPHOME TopH;
      GRAPHBOTHOME BotH;     
		INT WinWidth, WinHeight;
	public:
		FIXED SizeFactor;
		VOID Draw( HPS hps );
		VOID SetGraphics( INT Width, INT Height );
		GRAPHBOARD();
		
};
