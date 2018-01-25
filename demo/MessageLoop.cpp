#ifndef _MSC_VER
#include "MessageLoop.h"
#include "Queue.h"
#include <assert.h>
#include <atomic>
#include <thread>
#include <iostream>
#include <opencv2/highgui.hpp>

static CQueue<MSG> g_MessageQueue( 10 );

static void MouseCallback( int iEvent, int iX, int iY, int, void * )
{
	DWORD dwPos = ( (short) iX ) | ( ( (short) iY ) << 16 );

	switch( iEvent )
	{
	case cv::EVENT_LBUTTONDOWN:
		PostThreadMessage( 0, WM_LBUTTONDOWN, dwPos, 0 );
		break;
	case cv::EVENT_LBUTTONUP:
		PostThreadMessage( 0, WM_LBUTTONUP, dwPos, 0 );
		break;
	case cv::EVENT_MOUSEMOVE:
		PostThreadMessage( 0, WM_MOUSEMOVE, dwPos, 0 );
		break;
	}
}

static std::atomic_bool g_fKeyboardThread( true );
static void KeyboardThread( DWORD idThread )
{
	while( g_fKeyboardThread )
	{
		int iKey = cv::waitKey( 0 );
		PostThreadMessage( idThread, WM_KEYDOWN, iKey, 0 );
	}
}

static std::thread g_hKeyboardThread;
bool MessageInit( DWORD idThread, const std::string &sWindow )
{
	//g_hKeyboardThread = std::thread( KeyboardThread, idThread );
	cv::setMouseCallback( sWindow, MouseCallback, nullptr );
	return true;
}

void MessageTerminate( void )
{
	g_fKeyboardThread = false;
	g_hKeyboardThread.join( );
}

BOOL GetMessage( LPMSG lpMsg, void *hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax )
{
	assert( lpMsg );
	*lpMsg = g_MessageQueue.Pop_Front( );
	return true;
}

BOOL PeekMessage( LPMSG lpMsg, void *hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg )
{
	assert( lpMsg );
	return g_MessageQueue.Peek_Front( *lpMsg, wRemoveMsg );
}

BOOL PostThreadMessage( DWORD idThread, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	MSG msg;
	msg.message = Msg;
	msg.wParam = wParam;
	msg.lParam = lParam;
	g_MessageQueue.FEmplace_Back( msg );
	return true;
}

UINT MapVirtualKey( UINT uCode, UINT uMapType )
{
	printf( "Key: %x\n", uCode );
	int iReturn = 0;
	switch( uMapType )
	{
	case MAPVK_VK_TO_CHAR:
		switch( uCode )
		{
		case 0x10FFAB:	//Numpad_+
			iReturn = 43;
			break;
		case 0x10FFAD:	//Numpad_-
			iReturn = 45;
			break;
		}

		if( ( uCode & 0xFFFFFFF0 ) == 0x10FFB0 )
			iReturn = 48 + ( uCode & 0xF );	//Numpad_0 - Numpad_9

		if( ( uCode & 0xFFFFFF00 ) != 0x100000 )
			break;

		if( iReturn )
			break;

		iReturn = uCode & 0xFF;
		if( iReturn == 10 )
			iReturn = 13;
		break;
	case MAPVK_VK_TO_VSC:
		switch( uCode )
		{
		case 0x10FF50:		//Pos1
			iReturn = 71;
			break;
		case 0x10FF51:		//Arrow left
			iReturn = 75;
			break;
		case 0x10FF52:		//Arrow up
			iReturn = 72;
			break;
		case 0x10FF53:		//Arrow right
			iReturn = 77;
			break;
		case 0x10FF54:		//Arrow down
			iReturn = 80;
			break;
		case 0x10FF55:		//Img_Up
			iReturn = 73;
			break;
		case 0x10FF56:		//Img_Down
			iReturn = 81;
			break;
		case 0x10FF57:		//Home
			iReturn = 79;
			break;
		case 0x10FF63:		//Insert
			iReturn = 82;
			break;
		case 0x10FFFF:		//Delete
			iReturn = 83;
			break;
		}
		break;
	default:
		std::wcerr << "Unknown map type " << uMapType << std::endl;
	}

	return iReturn;
}
#endif
