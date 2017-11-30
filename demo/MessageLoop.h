#pragma once

#include <string>

#ifndef _MSC_VER
typedef int BOOL;
typedef unsigned int UINT;
typedef unsigned long long WPARAM;
typedef long long LPARAM;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned long long DWORD_PTR;
typedef long long LRESULT;

typedef struct
{
	long x;
	long y;
} POINT;

typedef struct
{
	UINT message;
	WPARAM wParam;
	LPARAM lParam;
	POINT pt;
} MSG, *LPMSG;

#define WM_QUIT			0x0012
#define WM_KEYDOWN		0x0100
#define WM_MOUSEMOVE	0x0200
#define WM_LBUTTONDOWN	0x0201
#define WM_LBUTTONUP	0x0202
#define WM_USER			0x0400

#define MAPVK_VK_TO_VSC		( 0 )
#define MAPVK_VK_TO_CHAR	( 2 )

#define PM_REMOVE	0x0001

#define LOWORD( l )			( (WORD) ( ( (DWORD_PTR) ( l ) ) & 0xffff ) )
#define HIWORD( l )			( (WORD) ( ( ( (DWORD_PTR) ( l ) ) >> 16 ) & 0xffff ) )
#define GET_X_LPARAM( lp )	( (int)(short) LOWORD( lp ) )
#define GET_Y_LPARAM( lp )	( (int)(short) HIWORD( lp ) )

bool MessageInit( DWORD idThread, const std::string &sWindow );
void MessageTerminate( void );

BOOL GetMessage( LPMSG lpMsg, void *hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax );
BOOL PeekMessage( LPMSG lpMsg, void *hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg );
BOOL PostThreadMessage( DWORD idThread, UINT Msg, WPARAM wParam, LPARAM lParam );
inline void PostQuitMessage( int nExitCode ) { }
UINT MapVirtualKey( UINT uCode, UINT uMapType );
inline BOOL TranslateMessage( const MSG *lpMsg ) { return true; }
inline LRESULT DispatchMessage( const MSG *lpMsg ) { return 0; }
#endif