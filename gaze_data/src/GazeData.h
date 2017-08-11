#pragma once

#include "GazeCapture.h"
#include "Ray.h"
#include "Render/Vector.h"
#include "Landmark.h"
#include "BBox.h"
#include <vector>

class CGazeData
{
public:
	static bool Init( const char *szFile );
	static void Destroy( void );
	static bool OpenOrCreate( const std::string &sFile );
	static std::vector<CGazeData> Load( const std::string &sFile );

	static std::vector<CGazeData> GetGazeData( std::vector<CGazeCapture> vecGaze, const char *szWindow );
	static double GetDistance( double dMeterDif, double dPixelDif, double dPixelDiagonal, double dTanFOV );
	static double GetPosition( double dDistance, double dPixelDif, double dPixelDiagonal, double dTanFOV );

	CGazeData( CLandmark &landmark, const CVector<3> &vec3Point, double dTanFOV );
	CGazeData( const CImage &img, const CBBox &boxFace, const CPoint &ptEyeLeft, const CPoint &ptEyeRight, const CVector<2> &vec2PYLeft, const CVector<2> &vec2PYRight, double dTanFOV );
	CGazeData( const CGazeData &other );
	bool DrawScenery( const char *szWindow );
	
	bool Write( void );

//private:
	static bool Load( std::vector<CGazeData> &vecData, const std::string &sLine );
	static CBBox FindTemplate( CImage &imgSrc, const CImage &imgTemplate );

	CRay m_rayEyeLeft;
	CRay m_rayEyeRight;
	CImage m_imgGaze;
	CBBox m_boxFace;
	CPoint m_ptEyeLeft;
	CPoint m_ptEyeRight;

	static double s_dEyeDistance;
	static FILE *s_pFile;
	static std::string s_sName;
	static std::string s_sDataPath;
	static std::string s_sRawPath;
	static unsigned int s_uCurrentImage;

	static const std::regex s_regex_name;
	static const std::regex s_regex_dist;
	static const std::regex s_regex_raw;
	static const std::regex s_regex_data;
	static const std::regex s_regex_line;
};

inline CGazeData::CGazeData( const CGazeData &other ) :
	m_rayEyeLeft( other.m_rayEyeLeft ),
	m_rayEyeRight( other.m_rayEyeRight ),
	m_imgGaze( other.m_imgGaze ),
	m_boxFace( other.m_boxFace ),
	m_ptEyeLeft( other.m_ptEyeLeft ),
	m_ptEyeRight( other.m_ptEyeRight )
{
	m_boxFace.TransferOwnership( m_imgGaze );
	m_ptEyeLeft.TransferOwnership( m_boxFace );
	m_ptEyeRight.TransferOwnership( m_boxFace );
}