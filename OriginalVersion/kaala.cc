/////////////////////////////////////////////////////////////////////
//	main program


#define INCL_DOSPROCESS
	// DosBeep()
#define INCL_WIN
//#define INCL_GPI
#include <os2.h>

#include "game.h"
#include "pmgame.h"
#include "defs.h"
//#include "kaala.h"

#define BEEP_WARN_FREQ	440U
#define BEEP_WARN_DUR	100U

// Prototypes

extern "C" MRESULT EXPENTRY WndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
inline VOID SetMenu( const USHORT usId, const BOOL fEnable );
inline VOID DisableMenu( const USHORT usId );
inline VOID EnableMenu( const USHORT usId );


// global variables

HAB hab;
HMQ hmq;
HWND hwndMain;
HWND hwndClient;
BOARD CalcBoard;
GRAPHBOARD GraphBoard;
PLAYER *Player, *CalcPlayer, PlayerInfo[2];


main(INT argc, CHAR *argv[], CHAR *envp[])
{
	QMSG qmsg;					// message queue
	ULONG fWndCtrlData;		// game Window style flags
	BOOL fRegistered;			//
//	RECTL RectWinPos;			// needed when resizing the window
//	LONG cxScreen;
//	LONG cyScreen;
	INT rc;

	static CHAR szClientClass[] = "kaala.child";
//	pszProgName = argv[0];		// get the full qualified program name

	if( ( hab = WinInitialize(0) ) == NULLHANDLE ){
		DosBeep( BEEP_WARN_FREQ, BEEP_WARN_DUR );
		return 1;
	}
	if( (hmq = WinCreateMsgQueue( hab, 0 ) ) == NULLHANDLE ){
		DosBeep( BEEP_WARN_FREQ, BEEP_WARN_DUR );
		DosBeep( BEEP_WARN_FREQ + 50, BEEP_WARN_DUR );
		WinTerminate( hab );
		return 1;
	}

	fRegistered = WinRegisterClass( hab, szClientClass, (PFNWP)WndProc,
						CS_SIZEREDRAW | CS_MOVENOTIFY, 0 );

	if( !fRegistered ){
		rc = WinGetLastError( hab );
		DosBeep( BEEP_WARN_FREQ, BEEP_WARN_DUR );
		DosBeep( BEEP_WARN_FREQ + 50, BEEP_WARN_DUR );
		DosBeep( BEEP_WARN_FREQ + 100, BEEP_WARN_DUR );
		WinDestroyMsgQueue( hmq );
		WinTerminate( hab );
		return 1;
	}

	fWndCtrlData = FCF_MINMAX		| FCF_TASKLIST		|
		            FCF_SYSMENU    | FCF_TITLEBAR    |
						FCF_SIZEBORDER | FCF_SHELLPOSITION|
						FCF_MENU       | FCF_ICON			|
						FCF_AUTOICON;
/*			 		   FCF_ACCELTABLE
*/ // use these later
	hwndMain =
		WinCreateStdWindow( HWND_DESKTOP,	// parent window
								  WS_ANIMATE,		// don't make window visible yet
								  &fWndCtrlData,	// window parameters, defined above
								  szClientClass,	// window class, def. above
								  "Kaala - The ancient game",	// Title Bar text
								  0,					// client window style
								  0,					// resources are bound within the .exe
								  IDR_MAIN,			// frame-window identifier (see .rc)
								  &hwndClient );	// output client-window handle

	if( hwndMain == NULLHANDLE ){
		DosBeep( BEEP_WARN_FREQ, BEEP_WARN_DUR);
		DosBeep( BEEP_WARN_FREQ + 50, BEEP_WARN_DUR);
		DosBeep( BEEP_WARN_FREQ + 100, BEEP_WARN_DUR);
		DosBeep( BEEP_WARN_FREQ + 150, BEEP_WARN_DUR);
		WinDestroyMsgQueue( hmq );
		WinTerminate( hab );
		return 1;
	}

	// Now we can make the active window visible:
	if( !WinShowWindow( hwndMain, TRUE ) ){
		DosBeep( BEEP_WARN_FREQ, BEEP_WARN_DUR );
		DosBeep( BEEP_WARN_FREQ + 50, BEEP_WARN_DUR );
		DosBeep( BEEP_WARN_FREQ + 100, BEEP_WARN_DUR );
		DosBeep( BEEP_WARN_FREQ + 150, BEEP_WARN_DUR );
		DosBeep( BEEP_WARN_FREQ + 200, BEEP_WARN_DUR );
	}

	// Now we can bring the window to the foreground
	// (it doesn't matter if this isn't successful)
	WinSetActiveWindow( HWND_DESKTOP, hwndMain );

	InitPlayerDefault();
	DisableMenu( IDM_GAMEUNDO );	// not allowed yet
	

	// get / dispatch message loop
	while( WinGetMsg( hab, (PQMSG)&qmsg, NULLHANDLE, 0L, 0L ) )
		WinDispatchMsg( hab, (PQMSG)&qmsg );

	// clean up
	WinDestroyWindow( hwndMain );
	WinDestroyMsgQueue( hmq );
	WinTerminate( hab );
	return 0;

}	// end of main
		
		
extern "C"
MRESULT EXPENTRY WndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
	HPS hps;
	RECTL rectl;
	POINTL ptl;
	HOLE *MovingHole;
//	BYTE MoveNumber;
	BOOL Again;
	static BOOL FirstMove = TRUE;
	
	switch( msg ){
		case WM_CREATE:
			break;
		case WM_CONTROL:
			break;
		case WM_QUIT:
			break;
		case WM_CLOSE:	// this message is sent before WM_QUIT
			// stop active threads
			break;
		case WM_ERASEBACKGROUND:
			return (MRESULT) FALSE;
			break;	

		case WM_PAINT:
			hps = WinBeginPaint( hwnd, 0, &rectl );
			WinFillRect( hps, &rectl, SYSCLR_WINDOW );	// clear background
			DosBeep( 700, 150 );
			GraphBoard.Draw( hps );
			DosBeep( 800, 150 );
			WinEndPaint( hps );
			return (MRESULT)0;
		case WM_SIZE:
			DosBeep( 500, 150 );
			GraphBoard.SetGraphics( SHORT1FROMMP( mp2 ), SHORT2FROMMP( mp2 ) );
			DosBeep( 600, 150 );
			break;
		case WM_BUTTON1CLICK:
			if( Player->Kind == HUMAN){
				ptl.x = (LONG)SHORT1FROMMP( mp1 );
				ptl.y = (LONG)SHORT2FROMMP( mp1 );
				MovingHole = GraphBoard.GetMovingHole( ptl, Player );
			} else {			// Player->Kind == COMPUTER
					MovingHole = GetComputerMove( FirstMove, GraphBoard );
			}
			if( MovingHole == NULL || !MovingHole->GetContents() )
				break;	// not a valid move
			Again = GraphBoard.Move( hwnd, MovingHole, Player );
			if( FirstMove && Again ){
				FirstMove = FALSE;
			} else {
				TogglePlayer( &Player );
				FirstMove = TRUE;
			}
			if( Player->Kind == COMPUTER )
				WinPostMsg( hwnd, WM_BUTTON1CLICK, 0, 0 );
				if( !GraphBoard.LastMove ) EnableMenu( IDM_GAMEUNDO );
					// after the first move LastMove is 0
			break;
		case WM_COMMAND:		// user has selected a menu item
			switch( SHORT1FROMMP( mp1 ) ){
				case IDM_FILEEXIT:		// good bye!
					WinPostMsg( hwnd, WM_CLOSE, 0, 0 );
					return 0;
				case IDM_GAMEOPTIONS:
					break;
				case IDM_GAMEUNDO:
					hps = WinGetPS( hwnd );	
					FirstMove = GraphBoard.Undo( hps, &Player );
					WinReleasePS( hps );
					if( GraphBoard.LastMove == -1 ) DisableMenu( IDM_GAMEUNDO );
						// Before the first move LastMove has the value -1
					break;
			}
			break;
				


		default:
			return (MRESULT)WinDefWindowProc( hwnd, msg, mp1, mp2 );
	}		// end switch( msg )
	return (MRESULT)WinDefWindowProc( hwnd, msg, mp1, mp2 );
}		// end MRESULT EXPENTRY WndProc()

/*************************************************************************\
 * Enable menu items
\*************************************************************************/
inline VOID EnableMenu( const USHORT usId )
{
	const HWND hwndMenu = WinWindowFromID( hwndMain, FID_MENU );
	(VOID)WinEnableMenuItem( hwndMenu, usId, TRUE );
}

/*************************************************************************\
 * Disable menu items
\*************************************************************************/
inline VOID DisableMenu( const USHORT usId )
{
	const HWND hwndMenu = WinWindowFromID( hwndMain, FID_MENU );
	(VOID)WinEnableMenuItem( hwndMenu, usId, FALSE );
}

/*************************************************************************\
 * Enable/Disable menu items
\*************************************************************************/
inline VOID SetMenu( const USHORT usId, const BOOL fEnable )
{	
	const HWND hwndMenu = WinWindowFromID( hwndMain, FID_MENU );
	(VOID)WinEnableMenuItem( hwndMenu, usId, fEnable );
}

