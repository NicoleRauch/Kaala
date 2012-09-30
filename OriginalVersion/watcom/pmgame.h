/////////////////////////////////////////////////////////////////////


class GRAPHBOARD : public BOARD {
	protected:
		INT WinWidth, WinHeight;
		INT xOffset, yOffset;
		INT Diameter;
	public:
		FIXED SizeFactor;
		VOID Draw( HPS hps );
		VOID SetGraphics( INT Width, INT Height );
		GRAPHBOARD();
		HOLE *GetMovingHole( POINTL ptl, PLAYER *Player );
		INT Move( HWND hwnd, HOLE *MovePtr, PLAYER *Player );
		void OneUndo( HPS hps, PLAYER *Player );
		BOOL Undo( HPS hps, PLAYER **Player );
		
};
