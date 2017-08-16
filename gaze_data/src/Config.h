#pragma once

#include "Render/Vector.h"
#include <regex>

#ifdef _MSC_VER
#	define	WORKING_DIRECTORY		"D:\\Users\\Rainer\\Source\\Repos\\deep_learning\\gaze_data\\"
#	define	HAARCASCADE_DIRECTORY	"D:\\Users\\Rainer\\Documents\\Visual Studio 2015\\Opencv-3.2.0\\build\\etc\\haarcascades\\"
#else
#	define	WORKING_DIRECTORY		"/home/rainer/Dokumente/deep_learning/gaze_data/"
#	define	HAARCASCADE_DIRECTORY	"/usr/share/opencv/haarcascades/"
#endif

class CConfig
{
public:
	CConfig( void );
	CConfig( const CConfig &other );
	CConfig( const std::string &sFile );
	
	void Swap( CConfig &other );
	CConfig &operator=( const CConfig &other );
	
	CVector<3> vec3MonitorPos;
	CVector<3> vec3MonitorDim;

private:
	static const std::regex s_regexMonPos;
	static const std::regex s_regexMonDim;
};

inline CConfig::CConfig( void )
{

}

inline CConfig::CConfig( const CConfig &other ) :
	vec3MonitorPos( other.vec3MonitorPos ),
	vec3MonitorDim( other.vec3MonitorDim )
{

}