#pragma once

#include "Render/Vector.h"
#include <regex>

#ifdef _MSC_VER
#	define	WORKING_DIRECTORY		"Path\\deep_learning\\gaze_data\\"
#	define	HAARCASCADE_DIRECTORY	"Path\\Opencv-3.2.0\\build\\etc\\haarcascades\\"
#else
#	define	WORKING_DIRECTORY		"/home/rainer/Dokumente/deep_learning/gaze_data/"
#	define	HAARCASCADE_DIRECTORY	"/usr/share/opencv/haarcascades/"
#endif

class CConfig
{
public:
	CConfig( void ) = default;
	CConfig( const std::string &sFile );
	
	CConfig( const CConfig &other ) = default;
	CConfig &operator=( const CConfig &other ) = default;

	CConfig( CConfig &&other ) = default;
	CConfig &operator=( CConfig &&other ) = default;
	
	CVector<3> vec3MonitorPos;
	CVector<3> vec3MonitorDim;

private:
	static const std::regex s_regexMonPos;
	static const std::regex s_regexMonDim;
};