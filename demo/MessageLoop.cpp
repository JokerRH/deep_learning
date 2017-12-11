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
	g_hKeyboardThread = std::thread( KeyboardThread, idThread );
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
	g_MessageQueue.Emplace_Back( msg );
	return true;
}

UINT MapVirtualKey( UINT uCode, UINT uMapType )
{
	std::wcout << "Key: " << uCode << std::endl;
	int iReturn = 0;
	switch( uMapType )
	{
	case MAPVK_VK_TO_CHAR:
		break;
	case MAPVK_VK_TO_VSC:
		break;
	default:
		std::wcerr << "Unknown map type " << uMapType << std::endl;
	}

	return iReturn;
}
#endif