#pragma once

#include <vector>

#ifdef _MSC_VER
#	include <EDSDK.h>
#endif

class CCanon
{
public:
	static bool Init( void );
	static void Terminate( void );
	static bool IsInitialized( void );
	static std::vector<std::string> GetCameraList( void );

private:
	static bool s_fInitialized;
};

inline bool CCanon::IsInitialized( void )
{
	return s_fInitialized;
}