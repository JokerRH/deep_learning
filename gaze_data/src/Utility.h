#pragma once

#include <opencv2\core.hpp>
#ifdef _MSC_VER
#	include <Windows.h>
#endif

class CUtility
{
public:
	static unsigned char WaitKey( unsigned int uTime );
	static void Cls( void );
	static unsigned char GetChar( void );

private:
	CUtility( void ) = delete;
	CUtility( const CUtility & ) = delete;
};