/**
\file
\author Rainer Heinelt
\brief CData implementation
*/

#include "Data.h"
#include "Compat.h"
#include "Render/Transformation.h"
#include "Render/Ray.h"
#include <iostream>
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>

#undef LoadImage
#undef max
#undef min

cv::CascadeClassifier CData::s_FaceCascade;

/**
\fn static bool CData::Init( const std::wstring &sPath )
\brief Initializes this class
\param[in]	sPath	Path to the OpenCV Haarcascade folder
\retval		true	Class sucessfully initialized
\retval		false	Error initializing (failed to load Haarcascade)
\asdf

Loads the OpenCV Haarcascade for face detection (\see CData::s_FaceCascade)
*/
bool CData::Init( const filestring_t &sPath )
{
	if( !s_FaceCascade.load( compat::ToString( compat::PathCombine_d( sPath, CFSTR( "haarcascade_frontalface_default.xml" ) ) ) ) )
	{
		std::wcerr << "Unable to load face cascade" << std::endl;
		return false;
	}

	return true;
}

CTransformation CData::GetFaceTransformation( void ) const
{
	CVector<3> vec3EyesX = vec3EyeRight - vec3EyeLeft;
	double dScale = vec3EyesX.Abs( );
	vec3EyesX /= dScale;
	CVector<3> vec3EyesY = vec3EyesX.CrossProduct( CVector<3>( { 0, 0, -1 } ) ).Normalize( );
	CVector<3> vec3EyesZ = vec3EyesY.CrossProduct( vec3EyesX ).Normalize( );

	return CTransformation::GetTRSMatrix( ( vec3EyeLeft + vec3EyeRight ) / 2.0, vec3EyesX, vec3EyesY, vec3EyesZ, dScale );
}

CData::CData( const cv::Mat &matImage, const cv::Rect &rectFace, const filestring_t &sPath ) :
	matImage( matImage.clone( ) ),
	rectFace( rectFace )
{
	if( sPath.empty( ) )
		return;

	sImage = compat::PathFindFileName_d( sPath );
}