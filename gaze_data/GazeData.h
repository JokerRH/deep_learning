#pragma once

#include "GazeCapture.h"
#include "Ray.h"
#include "Vector.h"
#include <vector>

class CGazeData
{
public:
	static std::vector<CGazeData> GetGazeData( std::vector<CGazeCapture> vecGaze, double dEyeDistance, double dFOV, CVector<3> vec3ScreenTL, CVector<3> vec3ScreenDim, const char *szWindow );
	static double GetDistance( double dMeterDif, double dPixelDif, double dPixelDiagonal, double dTanFOV );
	static double GetPosition( double dDistance, double dPixelDif, double dPixelDiagonal, double dTanFOV );
	CGazeData( const CVector<3> &vec3Point, const CVector<3> &vec3EyeLeft, const CVector<3> &vec3EyeRight );
	CGazeData( const CGazeData &other );
	bool DrawScenery( const char *szWindow );

private:

	CRay m_rayEyeLeft;
	CRay m_rayEyeRight;
};

inline CGazeData::CGazeData( const CVector<3> &vec3Point, const CVector<3> &vec3EyeLeft, const CVector<3> &vec3EyeRight ) :
	m_rayEyeLeft( vec3EyeLeft, vec3Point - vec3EyeLeft ),
	m_rayEyeRight( vec3EyeRight, vec3Point - vec3EyeRight )
{

}

inline CGazeData::CGazeData( const CGazeData &other ) :
	m_rayEyeLeft( other.m_rayEyeLeft ),
	m_rayEyeRight( other.m_rayEyeRight )
{

}