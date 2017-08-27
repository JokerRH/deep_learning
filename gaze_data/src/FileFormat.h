#pragma once

#include "Render/Vector.h"
#include <string>
#include <regex>
#include <fstream>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>

class CGazeData_Set
{
public:
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

	static CGazeData_Set LoadList( const std::string &sFile );
	CGazeData_Set( const std::vector<gazedata> &vecData, const std::string &sName, double dEyeDistance, const std::string &sDataPath, const std::string &sRawPath );
	CGazeData_Set( void );
	CGazeData_Set( const CGazeData_Set &other );
	CGazeData_Set &operator=( const CGazeData_Set &other );

	CGazeData_Set( CGazeData_Set &&other );
	CGazeData_Set &operator=( CGazeData_Set &&other );
	~CGazeData_Set( void );

	void ResetIterator( void );
	gazedata *GetNext( void );
	bool OpenWrite( const std::string &sFile );
	void Write( const gazedata &data, unsigned uPrecision = std::numeric_limits< double >::max_digits10 );
	void CloseWrite( void );
	void Sort( void );
	unsigned CheckDuplicates( bool fRemove );
	void Shuffle( void );

	bool WriteHeader( const std::string &sFile ) const;
	bool WriteAll( const std::string &sFile ) const;
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

	static const std::regex s_regex_name;
	static const std::regex s_regex_dist;
	static const std::regex s_regex_datapath;
	static const std::regex s_regex_rawpath;
	static const std::regex s_regex_data;
};

inline CGazeData_Set::gazedata::gazedata( time_t time, unsigned uImage, double dFOV, const CVector<2> &vec2EyeLeft, const CVector<2> &vec2PYLeft, const CVector<2> &vec2EyeRight, const CVector<2> &vec2PYRight ) :
	time( time ),
	uImage( uImage ),
	dFOV( dFOV ),
	vec2EyeLeft( vec2EyeLeft ),
	vec2PYLeft( vec2PYLeft ),
	vec2EyeRight( vec2EyeRight ),
	vec2PYRight( vec2PYRight )
{

}

inline CGazeData_Set::CGazeData_Set( void ) :
	sName( ),
	dEyeDistance( 0 ),
	sDataPath( ),
	sRawPath( )
{

}

inline CGazeData_Set::~CGazeData_Set( void )
{
	pthread_spin_destroy( &m_spinIterator );
}

inline CGazeData_Set::CGazeData_Set( const CGazeData_Set &other ) :
	vecData( other.vecData ),
	sName( other.sName ),
	dEyeDistance( other.dEyeDistance ),
	sDataPath( other.sDataPath ),
	sRawPath( other.sRawPath )
{
	pthread_spin_init( &m_spinIterator, PTHREAD_PROCESS_PRIVATE );
	ResetIterator( );
}

inline CGazeData_Set &CGazeData_Set::operator=( const CGazeData_Set &other )
{
	this->~CGazeData_Set( );
	new( this ) CGazeData_Set( other );
	return *this;
}

inline CGazeData_Set::CGazeData_Set( CGazeData_Set &&other ) :
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

inline CGazeData_Set &CGazeData_Set::operator=( CGazeData_Set &&other )
{
	this->~CGazeData_Set( );
	new( this ) CGazeData_Set( std::move( other ) );
	return *this;
}

inline void CGazeData_Set::ResetIterator( void )
{
	pthread_cleanup_push( ( void( *)( void * ) ) pthread_spin_unlock, (void *) &m_spinIterator );
	pthread_spin_lock( &m_spinIterator );
	m_itData = vecData.begin( );
	pthread_cleanup_pop( 1 );
}

inline bool CGazeData_Set::OpenWrite( const std::string &sFile )
{
	m_FileWrite.open( sFile, std::fstream::app );
	return m_FileWrite.is_open( );
}

inline void CGazeData_Set::CloseWrite( void )
{
	m_FileWrite.close( );
}

inline void CGazeData_Set::Sort( void )
{
	std::sort( vecData.begin( ), vecData.end( ), [ ]( const CGazeData_Set::gazedata &dataA, const CGazeData_Set::gazedata &dataB )->bool
		{
			return dataA.uImage < dataB.uImage;
		} );
}

inline void CGazeData_Set::Shuffle( void )
{
	std::random_shuffle( vecData.begin( ), vecData.end( ) );
}