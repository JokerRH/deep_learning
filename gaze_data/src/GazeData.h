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
	gazedata( time_t time, unsigned uImage, double dFOV, const CVector<2> &vec2EyeLeft, const CVector<2> &vec2PYLeft, const CVector<2> &vec2EyeRight, const CVector<2> &vec2PYRight );
	std::string ToString( unsigned uPrecision = std::numeric_limits< double >::max_digits10 ) const;
	std::string ToCSV( void ) const;

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
	gazedata_set( const gazedata_set &other );
	gazedata_set &operator=( const gazedata_set &other );

	gazedata_set( gazedata_set &&other );
	gazedata_set &operator=( gazedata_set &&other );
	~gazedata_set( void );
	
	void ResetIterator( void );
	gazedata *GetNext( void );
	bool OpenWrite( const std::string &sFile );
	void Write( const gazedata &data, unsigned uPrecision = std::numeric_limits< double >::max_digits10 );
	void CloseWrite( void );
	void Sort( void );
	unsigned RemoveDuplicates( void );
	void Shuffle( void );
	bool WriteAll( const std::string &sFile );
	bool Export( const std::string &sPath );

	std::vector<gazedata> vecData;
	const std::string sName;
	const double dEyeDistance;
	const std::string sDataPath;
	const std::string sRawPath;
	
private:
	pthread_spinlock_t m_spinIterator;
	std::vector<gazedata>::iterator m_itData;
	std::fstream m_FileWrite;
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
	static bool Export( const std::string &sFile, const std::string &sPath, double dTrainValRatio = 2.0 / 3.0 );

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

	std::string ToString( unsigned int uPrecvecDaision = std::numeric_limits< double >::max_digits10 ) const;
	void WriteImage( const std::string &sPath ) const;
	gazedata ToData( void ) const;

private:
	CGazeData( const gazedata_set &set, const gazedata &data );
	static void *ReadThread( void * );
	static void *WriteThread( void * );
	static void *ReadRawThread( void * );
	static CBBox FindTemplate( CImage &imgSrc, const CImage &imgTemplate );

	CRay m_rayEyeLeft;
	CRay m_rayEyeRight;
	CImage m_imgGaze;
	CBBox m_boxFace;
	CPoint m_ptEyeLeft;
	CPoint m_ptEyeRight;
	unsigned int m_uImage;

	static gazedata_set s_DataSet;
	static CQueue<CGazeData> s_QueueRead;
	static CQueue<CGazeData> s_QueueWrite;
	static std::vector<pthread_t> s_vecThreadRead;
	static std::vector<pthread_t> s_vecThreadWrite;
	static unsigned s_uNextImage;

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

inline gazedata::gazedata( time_t time, unsigned uImage, double dFOV, const CVector<2> &vec2EyeLeft, const CVector<2> &vec2PYLeft, const CVector<2> &vec2EyeRight, const CVector<2> &vec2PYRight ) :
	time( time ),
	uImage( uImage ),
	dFOV( dFOV ),
	vec2EyeLeft( vec2EyeLeft ),
	vec2PYLeft( vec2PYLeft ),
	vec2EyeRight( vec2EyeRight ),
	vec2PYRight( vec2PYRight )
{

}

inline gazedata_set::gazedata_set( void ) :
	sName( ),
	dEyeDistance( 0 ),
	sDataPath( ),
	sRawPath( )
{

}

inline gazedata_set::~gazedata_set( void )
{
	pthread_spin_destroy( &m_spinIterator );
}

inline gazedata_set::gazedata_set( const gazedata_set &other ) :
	vecData( other.vecData ),
	sName( other.sName ),
	dEyeDistance( other.dEyeDistance ),
	sDataPath( other.sDataPath ),
	sRawPath( other.sRawPath )
{
	pthread_spin_init( &m_spinIterator, PTHREAD_PROCESS_PRIVATE );
	ResetIterator( );
}

inline gazedata_set &gazedata_set::operator=( const gazedata_set &other )
{
	this->~gazedata_set( );
	new( this ) gazedata_set( other );
	return *this;
}

inline gazedata_set::gazedata_set( gazedata_set &&other ) :
	vecData( std::move( other.vecData ) ),
	sName( std::move( other.sName ) ),
	dEyeDistance( std::move( other.dEyeDistance ) ),
	sDataPath( std::move( other.sDataPath ) ),
	sRawPath( std::move( other.sRawPath ) ),
	m_spinIterator( std::move( other.m_spinIterator ) ),
	m_itData( std::move( other.m_itData ) ),
	m_FileWrite( std::move( other.m_FileWrite ) )
{
	ResetIterator( );
}

inline gazedata_set &gazedata_set::operator=( gazedata_set &&other )
{
	this->~gazedata_set( );
	new( this ) gazedata_set( std::move( other ) );
	return *this;
}

inline void gazedata_set::ResetIterator( void )
{
	pthread_cleanup_push( (void (*)(void *)) pthread_spin_unlock, (void *) &m_spinIterator );
		pthread_spin_lock( &m_spinIterator );
		m_itData = vecData.begin( );
	pthread_cleanup_pop( 1 );
}

inline gazedata *gazedata_set::GetNext( void )
{
	gazedata *pData = nullptr;
	pthread_cleanup_push( (void (*)(void *)) pthread_spin_unlock, (void *) &m_spinIterator );
		pthread_spin_lock( &m_spinIterator );
		if( m_itData < vecData.end( ) )
		{
			pData = &( *m_itData );
			m_itData++;
		}
	pthread_cleanup_pop( 1 );
	
	return pData;
}

inline bool gazedata_set::OpenWrite( const std::string &sFile )
{
	m_FileWrite.open( sFile, std::fstream::app );
	return m_FileWrite.is_open( );
}

inline void gazedata_set::CloseWrite( void )
{
	m_FileWrite.close( );
}

inline void gazedata_set::Sort( void )
{
	std::sort( vecData.begin( ), vecData.end( ), sort_lt( ) );
}

inline void gazedata_set::Shuffle( void )
{
	std::random_shuffle( vecData.begin( ), vecData.end( ) );
}