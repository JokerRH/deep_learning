#pragma once

#include "GazeCapture.h"
#include "Ray.h"
#include "Render/Vector.h"
#include "Landmark.h"
#include "BBox.h"
#include "Queue.h"
#include <vector>
#include <fstream>

class CGazeData
{
public:
	static bool OpenWrite( const std::string &sFile );
	static void CloseWrite( void );
	static bool OpenRead( const std::string &sFile );
	static bool OpenReadRaw( const std::string &sFile );
	static bool ReadAsync( CGazeData &val );
	void WriteAsync( void );
	static bool ReadRawAsync( CGazeData &val );

	static std::vector<CGazeData> GetGazeData( CGazeCapture &capture );
	static double GetDistance( double dMeterDif, double dPixelDif, double dPixelDiagonal, double dTanFOV );
	static double GetPosition( double dDistance, double dPixelDif, double dPixelDiagonal, double dTanFOV );

	CGazeData( CLandmark &landmark, const CVector<3> &vec3Point, double dTanFOV, unsigned int uImage );
	CGazeData( void );

	CGazeData( const CGazeData &other );
	CGazeData &operator=( const CGazeData &other );

	CGazeData( CGazeData &&other ) = default;
	CGazeData &operator=( CGazeData &&other ) = default;
	
	bool Adjust( const char *szWindow );
	
	bool DrawScenery( const char *szWindow );
	void RandomizeFace( double dMaxScale );

	std::string ToString( unsigned int uPrecision = std::numeric_limits< double >::max_digits10 ) const;
	void WriteImage( void ) const;

private:
	CGazeData( std::string sLine );
	static void *ReadThread( void *pArgs );
	static void *WriteThread( void *pArgs );
	static void *ReadRawThread( void *pArgs );
	static CBBox FindTemplate( CImage &imgSrc, const CImage &imgTemplate );

	CRay m_rayEyeLeft;
	CRay m_rayEyeRight;
	CImage m_imgGaze;
	CBBox m_boxFace;
	CPoint m_ptEyeLeft;
	CPoint m_ptEyeRight;
	unsigned int m_uImage;

	static std::fstream s_File;
	static CQueue<CGazeData> s_Queue;
	static CQueue<CGazeData> s_QueueRaw;
	static std::vector<pthread_t> s_vecThread;
	static std::vector<pthread_t> s_vecThreadRaw;

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
	
	friend CQueue<CGazeData>;
};

inline CGazeData::CGazeData( const CGazeData &other ) :
	m_rayEyeLeft( other.m_rayEyeLeft ),
	m_rayEyeRight( other.m_rayEyeRight ),
	m_imgGaze( other.m_imgGaze ),
	m_boxFace( other.m_boxFace ),
	m_ptEyeLeft( other.m_ptEyeLeft ),
	m_ptEyeRight( other.m_ptEyeRight ),
	m_uImage( other.m_uImage )
{
	m_boxFace.TransferOwnership( m_imgGaze );
	m_ptEyeLeft.TransferOwnership( m_boxFace );
	m_ptEyeRight.TransferOwnership( m_boxFace );
}

inline CGazeData &CGazeData::operator=( const CGazeData &other )
{
	operator=( std::move( CGazeData( other ) ) );
	return *this;
}

inline CGazeData::CGazeData( void ) :
	m_uImage( -1 )
{

}