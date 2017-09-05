#pragma once

#include "GazeCapture.h"
#include "Ray.h"
#include "Render/Vector.h"
#include "Landmark.h"
#include "BBox.h"
#include "Queue.h"
#include "FileFormat.h"
#include <vector>
#include <fstream>
#include <algorithm>

class CGazeData
{
public:
	static bool OpenWrite( const std::string &sFile, bool fCreateDataFolder = true );
	static void CloseWrite( void );
	static bool OpenRead( const std::string &sFile );
	static bool OpenReadRaw( const std::string &sFileRaw, const std::string &sFile );
	static bool ReadAsync( CGazeData &val );
	void WriteAsync( void );
	static bool Export( const std::string &sFile, const std::string &sPath, unsigned int uTestBatchSize, double dTrainValRatio = 2.0 / 3.0 );
	static bool Import( const std::string &sPath, const std::string &sRaw );

	static std::vector<CGazeData> GetGazeData( CGazeCapture &capture );
	static double GetDistance( double dMeterDif, double dPixelDif, double dPixelDiagonal, double dTanFOV );
	static double GetTanFOV( double dMeterDif, double dPixelDif, double dPixelDiagonal, double dDistance );
	static double GetPosition( double dDistance, double dPixelDif, double dPixelDiagonal, double dTanFOV );

	CGazeData( CLandmark &landmark, const CVector<3> &vec3Point, double dTanFOV, unsigned int uImage );
	CGazeData( void );

	CGazeData( const CGazeData &other );
	CGazeData &operator=( const CGazeData &other );

	CGazeData( CGazeData &&other ) = default;
	CGazeData &operator=( CGazeData &&other );
	
	bool Adjust( const char *szWindow );
	
	bool DrawScenery( const char *szWindow );
	void RandomizeFace( double dMaxScale );

	std::string ToString( unsigned int uPrecvecDaision = std::numeric_limits< double >::max_digits10 ) const;
	void WriteImage( const std::string &sPath ) const;
	CGazeData_Set::gazedata ToData( void ) const;
	
	static CGazeData_Set s_DataSetRead;
	static CGazeData_Set s_DataSetWrite;

private:
	CGazeData( const CGazeData_Set &set, const CGazeData_Set::gazedata &data );
	static void *ReadThread( void * );
	static void *WriteThread( void * );
	static void *ReadRawThread( void * );
	static CBBox FindTemplate( CImage &imgSrc, const CImage &imgTemplate );
	static void CheckDuplicates( CGazeData_Set &dataset, const std::string &sFile );

	CRay m_rayEyeLeft;
	CRay m_rayEyeRight;
	CImage m_imgGaze;
	CBBox m_boxFace;
	CPoint m_ptEyeLeft;
	CPoint m_ptEyeRight;
	unsigned int m_uImage;

	static CQueue<CGazeData> s_QueueRead;
	static CQueue<CGazeData> s_QueueWrite;
	static std::vector<pthread_t> s_vecThreadRead;
	static std::vector<pthread_t> s_vecThreadWrite;
	static unsigned s_uNextImage;

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
	if( m_uImage == (unsigned) -1 )
		return;

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

inline CGazeData &CGazeData::operator=( CGazeData &&other )
{
	this->~CGazeData( );
	new( this ) CGazeData( std::move( other ) );
	return *this;
}