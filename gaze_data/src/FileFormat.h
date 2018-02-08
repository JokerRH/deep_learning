#pragma once

#include "Render/Vector.h"
#include <string>
#include <regex>
#include <fstream>
#include <atomic>

//===========================================================================
// CGazeCapture_Set
//===========================================================================
class CGazeCapture_Set
{
public:
	struct gazecapture
	{
		gazecapture( std::string sLine );
		gazecapture( time_t time, unsigned uImage, double dFOV, const CVector<3> &vec3Gaze, const std::string &sImage = std::string( ) );
		std::string ToString( unsigned uPrecision = std::numeric_limits< double >::max_digits10 ) const;

		time_t time;
		unsigned int uImage;
		double dFOV;
		CVector<3> vec3Gaze;
		std::string sImage;

	private:
		static const std::regex s_regLine;
	};

	static CGazeCapture_Set LoadList( const std::string &sFile );
	CGazeCapture_Set( const std::vector<gazecapture> &vecData, const std::string &sName, double dEyeDistance, const std::string &sDataPath );
	CGazeCapture_Set( void );
	CGazeCapture_Set( const CGazeCapture_Set &other );
	CGazeCapture_Set &operator=( const CGazeCapture_Set &other );

	CGazeCapture_Set( CGazeCapture_Set &&other );
	CGazeCapture_Set &operator=( CGazeCapture_Set &&other );
	~CGazeCapture_Set( void );

	void ResetIterator( void );
	gazecapture *GetNext( void );
	bool OpenWrite( const std::string &sFile );
	void Write( const gazecapture &data, unsigned uPrecision = std::numeric_limits< double >::max_digits10 );
	void CloseWrite( void );
	void Sort( void );
	unsigned CheckDuplicates( bool fRemove );
	void Shuffle( void );
	void GetCount( unsigned &uTotal, unsigned &uCurrent );

	bool WriteHeader( const std::string &sFile ) const;
	bool WriteAll( const std::string &sFile ) const;

	std::vector<gazecapture> vecData;
	const std::string sName;
	const double dEyeDistance;
	const std::string sDataPath;
	const std::string sRawPath;

private:
	std::atomic_flag m_spinIterator = ATOMIC_FLAG_INIT;
	std::vector<gazecapture>::iterator m_itData;
	std::fstream m_FileWrite;
	unsigned int m_uRead;

	static const std::regex s_regName;
	static const std::regex s_regDist;
	static const std::regex s_regDataPath;
	static const std::regex s_regData;
};

//===========================================================================
// CGazeData_Set
//===========================================================================
class CGazeData_Set
{
public:
	struct gazedata
	{
		gazedata( std::string sLine );
		gazedata( time_t time, unsigned uImage, double dFOV, const CVector<2> &vec2EyeLeft, const CVector<2> &vec2PYLeft, const CVector<2> &vec2EyeRight, const CVector<2> &vec2PYRight );
		std::string ToString( unsigned uPrecision = std::numeric_limits< double >::max_digits10 ) const;
		std::string ToCSV( unsigned uPrecision = std::numeric_limits< double >::max_digits10 ) const;

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
	void GetCount( unsigned &uTotal, unsigned &uCurrent );

	bool WriteHeader( const std::string &sFile ) const;
	bool WriteAll( const std::string &sFile ) const;
	bool Export( const std::string &sPath );

	std::vector<gazedata> vecData;
	const std::string sName;
	const double dEyeDistance;
	const std::string sDataPath;
	const std::string sRawPath;

private:
	std::atomic_flag m_spinIterator = ATOMIC_FLAG_INIT;
	std::vector<gazedata>::iterator m_itData;
	std::fstream m_FileWrite;
	unsigned int m_uRead;

	static const std::regex s_regex_name;
	static const std::regex s_regex_dist;
	static const std::regex s_regex_datapath;
	static const std::regex s_regex_rawpath;
	static const std::regex s_regex_data;
};

//===========================================================================
// CGazeCapture_Set
//===========================================================================
inline CGazeCapture_Set::gazecapture::gazecapture( time_t time, unsigned uImage, double dFOV, const CVector<3> &vec3Gaze, const std::string &sImage ) :
	time( time ),
	uImage( uImage ),
	dFOV( dFOV ),
	vec3Gaze( vec3Gaze ),
	sImage( sImage )
{

}

inline CGazeCapture_Set::CGazeCapture_Set( void ) :
	sName( ),
	dEyeDistance( 0 ),
	sDataPath( )
{

}

inline CGazeCapture_Set::~CGazeCapture_Set( void )
{

}

inline CGazeCapture_Set::CGazeCapture_Set( const CGazeCapture_Set &other ) :
	vecData( other.vecData ),
	sName( other.sName ),
	dEyeDistance( other.dEyeDistance ),
	sDataPath( other.sDataPath )
{
	ResetIterator( );
}

inline CGazeCapture_Set &CGazeCapture_Set::operator=( const CGazeCapture_Set &other )
{
	this->~CGazeCapture_Set( );
	new( this ) CGazeCapture_Set( other );
	return *this;
}

inline CGazeCapture_Set::CGazeCapture_Set( CGazeCapture_Set &&other ) :
	vecData( std::move( other.vecData ) ),
	sName( std::move( other.sName ) ),
	dEyeDistance( std::move( other.dEyeDistance ) ),
	sDataPath( std::move( other.sDataPath ) ),
	m_itData( std::move( other.m_itData ) ),
	m_FileWrite( std::move( other.m_FileWrite ) )
{
	ResetIterator( );
}

inline CGazeCapture_Set &CGazeCapture_Set::operator=( CGazeCapture_Set &&other )
{
	this->~CGazeCapture_Set( );
	new( this ) CGazeCapture_Set( std::move( other ) );
	return *this;
}

inline void CGazeCapture_Set::ResetIterator( void )
{
	while( m_spinIterator.test_and_set( std::memory_order_acquire ) );
	m_itData = vecData.begin( );
	m_uRead = 0;
	m_spinIterator.clear( std::memory_order_release );
}

inline bool CGazeCapture_Set::OpenWrite( const std::string &sFile )
{
	m_FileWrite.open( sFile, std::fstream::app );
	return m_FileWrite.is_open( );
}

inline void CGazeCapture_Set::CloseWrite( void )
{
	m_FileWrite.close( );
}

inline void CGazeCapture_Set::Sort( void )
{
	std::sort( vecData.begin( ), vecData.end( ), [ ]( const CGazeCapture_Set::gazecapture &dataA, const CGazeCapture_Set::gazecapture &dataB )->bool
	{
		return dataA.uImage < dataB.uImage;
	} );
}

inline void CGazeCapture_Set::Shuffle( void )
{
	std::random_shuffle( vecData.begin( ), vecData.end( ) );
}

inline void CGazeCapture_Set::GetCount( unsigned &uTotal, unsigned &uCurrent )
{
	while( m_spinIterator.test_and_set( std::memory_order_acquire ) );
	uTotal = (unsigned) vecData.size( );
	m_uRead = uCurrent;
	m_spinIterator.clear( std::memory_order_release );
}

//===========================================================================
// CGazeData_Set
//===========================================================================
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

}

inline CGazeData_Set::CGazeData_Set( const CGazeData_Set &other ) :
	vecData( other.vecData ),
	sName( other.sName ),
	dEyeDistance( other.dEyeDistance ),
	sDataPath( other.sDataPath ),
	sRawPath( other.sRawPath )
{
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
	while( m_spinIterator.test_and_set( std::memory_order_acquire ) );
	m_itData = vecData.begin( );
	m_uRead = 0;
	m_spinIterator.clear( std::memory_order_release );
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

inline void CGazeData_Set::GetCount( unsigned &uTotal, unsigned &uCurrent )
{
	while( m_spinIterator.test_and_set( std::memory_order_acquire ) );
	uTotal = (unsigned) vecData.size( );
	m_uRead = uCurrent;
	m_spinIterator.clear( std::memory_order_release );
}