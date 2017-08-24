#pragma once

#include "GazeCapture.h"
#include "Ray.h"
#include "Render/Vector.h"
#include "Landmark.h"
#include "BBox.h"
#include "Queue.h"
#include <vector>
#include <fstream>
#include <algorithm>

struct gazedata
{
	gazedata( std::string sLine );
	std::string ToString( unsigned int uPrecision = std::numeric_limits< double >::max_digits10 );

	time_t time;
	unsigned int uImage;
	double dFOV;
	CVector<2> vec2EyeLeft;
	CVector<2> vec2PYLeft;
	CVector<2> vec2EyeRight;
	CVector<2> vec2PYRight;
	
private:
	static const std::regex s_regex_line;
};

struct gazedata_set
{
	static gazedata_set LoadList( const std::string &sFile );
	gazedata_set( const std::vector<gazedata> &vecData, const std::string &sName, double dEyeDistance, const std::string &sDataPath, const std::string &sRawPath );
	gazedata_set( void );
	gazedata_set( const gazedata_set &other ) = default;
	gazedata_set &operator=( const gazedata_set &other ) = default;

	gazedata_set( gazedata_set &&other ) = default;
	gazedata_set &operator=( gazedata_set &&other );
	
	void Sort( void );
	void Shuffle( void );
	bool Write( const std::string &sFile );

	std::vector<gazedata> vecData;
	const std::string sName;
	const double dEyeDistance;
	const std::string sDataPath;
	const std::string sRawPath;
	
private:
	struct sort_lt
	{
		inline bool operator()( const gazedata& data1, const gazedata& data2 )
		{
			return ( data1.uImage < data2.uImage );
		}
	};
	
	static const std::regex s_regex_name;
	static const std::regex s_regex_dist;
	static const std::regex s_regex_datapath;
	static const std::regex s_regex_rawpath;
	static const std::regex s_regex_data;
};

class CGazeData
{
public:
	static bool OpenWrite( const std::string &sFile, bool fCreateDataFolder = true );
	static void CloseWrite( void );
	static bool OpenRead( const std::string &sFile );
	static bool OpenReadRaw( const std::string &sFileRaw, const std::string &sFile );
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
	CGazeData &operator=( CGazeData &&other );
	
	bool Adjust( const char *szWindow );
	
	bool DrawScenery( const char *szWindow );
	void RandomizeFace( double dMaxScale );

	std::string ToString( unsigned int uPrecision = std::numeric_limits< double >::max_digits10 ) const;
	void WriteImage( void ) const;

private:
	CGazeData( const gazedata_set &set, const gazedata &data );
	static void *ReadThread( gazedata_set *pDataSet );
	static void *WriteThread( void *pArgs );
	static void *ReadRawThread( gazedata_set *pDataSet );
	static CBBox FindTemplate( CImage &imgSrc, const CImage &imgTemplate );

	CRay m_rayEyeLeft;
	CRay m_rayEyeRight;
	CImage m_imgGaze;
	CBBox m_boxFace;
	CPoint m_ptEyeLeft;
	CPoint m_ptEyeRight;
	unsigned int m_uImage;

	static gazedata_set s_DataSet;
	static std::fstream s_File;
	static std::fstream s_FileWrite;
	static CQueue<CGazeData> s_Queue;
	static CQueue<CGazeData> s_QueueWrite;
	static CQueue<CGazeData> s_QueueRaw;
	static std::vector<pthread_t> s_vecThread;
	static std::vector<pthread_t> s_vecThreadWrite;
	static std::vector<pthread_t> s_vecThreadRaw;
	static std::vector<unsigned int> s_vecIgnore;

	static double s_dEyeDistance;
	static FILE *s_pFile;
	static std::string s_sName;
	static std::string s_sDataPath;
	static std::string s_sRawPath;
	static unsigned int s_uCurrentImage;
	
	friend CQueue<CGazeData>;
	friend gazedata;
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

inline CGazeData &CGazeData::operator=( CGazeData &&other )
{
	this->~CGazeData( );
	new( this ) CGazeData( std::move( other ) );
	return *this;
}

inline gazedata_set::gazedata_set( void ) :
	sName( ),
	dEyeDistance( 0 ),
	sDataPath( ),
	sRawPath( )
{

}

inline gazedata_set &gazedata_set::operator=( gazedata_set &&other )
{
	this->~gazedata_set( );
	new( this ) gazedata_set( other );
	return *this;
}

inline void gazedata_set::Sort( void )
{
	std::sort( vecData.begin( ), vecData.end( ), sort_lt( ) );
}

inline void gazedata_set::Shuffle( void )
{
	std::random_shuffle( vecData.begin( ), vecData.end( ) );
}