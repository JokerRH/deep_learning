#include "FileFormat.h"
#include <time.h>

const std::regex CGazeCapture_Set::s_regName( R"a(name=([\s\S]*).*)a" );
const std::regex CGazeCapture_Set::s_regDist( R"a(dist=((?:\d+(?:\.\d+)?)|(?:\.\d+))cm.*)a" );
const std::regex CGazeCapture_Set::s_regDataPath( R"a(data=([\s\S]*).*)a" );
const std::regex CGazeCapture_Set::s_regData( R"a(data:.*)a" );
const std::regex CGazeCapture_Set::gazecapture::s_regLine( R"a((\d{4})-(\d{2})-(\d{2})\s+(\d{2}):(\d{2}):(\d{2})\s+(\d+)\s+((?:\d+(?:\.\d+)?)|(?:\.\d+))\s+\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\)\s+(\S+)?.*)a" );

const std::regex CGazeData_Set::s_regex_name( R"a(name=([\s\S]*).*)a" );
const std::regex CGazeData_Set::s_regex_dist( R"a(dist=((?:\d+(?:\.\d+)?)|(?:\.\d+))cm.*)a" );
const std::regex CGazeData_Set::s_regex_datapath( R"a(data=([\s\S]*).*)a" );
const std::regex CGazeData_Set::s_regex_rawpath( R"a(raw=([\s\S]*).*)a" );
const std::regex CGazeData_Set::s_regex_data( R"a(data:.*)a" );
const std::regex CGazeData_Set::gazedata::s_regex_line( R"a((\d{4})-(\d{2})-(\d{2})\s+(\d{2}):(\d{2}):(\d{2})\s+(\d+)\s+((?:\d+(?:\.\d+)?)|(?:\.\d+))\s+\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\)@\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\)\s+\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\)@\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\).*)a" );

//===========================================================================
// CGazeCapture_Set
//===========================================================================

CGazeCapture_Set::gazecapture::gazecapture( std::string sLine )
{
	std::smatch match;
	std::replace( sLine.begin( ), sLine.end( ), '\r', ' ' );
	std::regex_match( sLine, match, s_regLine );
	if( !match.size( ) )
		throw 0;

	{
		struct tm timeinfo = { 0 };
		timeinfo.tm_sec = std::stoi( match[ 6 ].str( ) );
		timeinfo.tm_min = std::stoi( match[ 5 ].str( ) );
		timeinfo.tm_hour = std::stoi( match[ 4 ].str( ) );
		timeinfo.tm_mday = std::stoi( match[ 3 ].str( ) );
		timeinfo.tm_mon = std::stoi( match[ 2 ].str( ) ) - 1;
		timeinfo.tm_year = std::stoi( match[ 1 ].str( ) ) - 1900;
		time = mktime( &timeinfo );
	}

	uImage = std::stoul( match[ 7 ].str( ) );
	dFOV = std::stod( match[ 8 ].str( ) );
	vec3Gaze = CVector<3>( { std::stod( match[ 9 ].str( ) ), std::stod( match[ 10 ].str( ) ), std::stod( match[ 11 ].str( ) ) } );
	if( match[ 12 ].matched )
		sImage = match[ 12 ].str( );
	else
		sImage = nullptr;
}

std::string CGazeCapture_Set::gazecapture::ToString( unsigned int uPrecision ) const
{
	std::ostringstream out;
	out.setf( std::ios_base::fixed, std::ios_base::floatfield );
	out.precision( uPrecision );

	//Write date
	{
		struct tm timeinfo;
#ifdef _MSC_VER
		localtime_s( &timeinfo, &time );
#else
		localtime_r( &time, &timeinfo );
#endif
		char szDate[ 20 ];
		strftime( szDate, 20, "%F %T", &timeinfo ); //YYYY-MM-DD HH:MM:SS
		out << szDate;
	}

	out << " " << uImage << " " << dFOV;
	out << " (" << vec3Gaze[ 0 ] << ", " << vec3Gaze[ 1 ] << ", " << vec3Gaze[ 2 ] << ")";
	out << " " << sImage;
	return out.str( );
}

CGazeCapture_Set CGazeCapture_Set::LoadList( const std::string &sFile )
{
	std::vector<gazecapture> vecData;
	std::string sName;
	double dEyeDistance;
	std::string sDataPath;

	std::fstream file( sFile, std::fstream::in );
	if( !file.is_open( ) )
	{
		fprintf( stderr, "Unable to open file \"%s\"\n", sFile.c_str( ) );
		throw 0;
	}

	std::smatch match;
	unsigned char fFound = 0;
	std::string sLine;
	while( std::getline( file, sLine ) )
	{
		std::regex_match( sLine, match, s_regName );
		if( match.size( ) )
		{
			sName = match[ 1 ].str( );
			fFound |= 1;
			continue;
		}

		std::regex_match( sLine, match, s_regDist );
		if( match.size( ) )
		{
			dEyeDistance = std::stod( match[ 1 ].str( ) ) / 100;
			fFound |= 2;
			continue;
		}

		std::regex_match( sLine, match, s_regDataPath );
		if( match.size( ) )
		{
			sDataPath = match[ 1 ].str( );
			fFound |= 4;
			continue;
		}

		std::regex_match( sLine, match, s_regData );
		if( match.size( ) )
		{
			fFound |= 8;
			break;	//Start of data
		}
	}

	if( fFound != 15 )
	{
		fprintf( stderr, "File \"%s\" is missing fields\n", sFile.c_str( ) );
		file.close( );
		throw 1;
	}

	while( std::getline( file, sLine ) )
	{
		try
		{
			vecData.emplace_back( sLine );
		}
		catch( int )
		{
			continue;
		}
	}
	file.close( );
	return CGazeCapture_Set( vecData, sName, dEyeDistance, sDataPath );
}

CGazeCapture_Set::CGazeCapture_Set( const std::vector<gazecapture> &vecData, const std::string &sName, double dEyeDistance, const std::string &sDataPath ) :
	vecData( vecData ),
	sName( sName ),
	dEyeDistance( dEyeDistance ),
	sDataPath( sDataPath )
{
	pthread_spin_init( &m_spinIterator, PTHREAD_PROCESS_PRIVATE );
	ResetIterator( );
}

CGazeCapture_Set::gazecapture *CGazeCapture_Set::GetNext( void )
{
	gazecapture *pData = nullptr;
	pthread_cleanup_push( ( void( *)( void * ) ) pthread_spin_unlock, (void *) &m_spinIterator );
	pthread_spin_lock( &m_spinIterator );
	if( m_itData < vecData.end( ) )
	{
		pData = &( *m_itData );
		m_itData++;
	}
	pthread_cleanup_pop( 1 );

	return pData;
}

void CGazeCapture_Set::Write( const gazecapture &data, unsigned uPrecision )
{
	pthread_cleanup_push( ( void( *)( void * ) ) pthread_spin_unlock, (void *) &m_spinIterator );
	pthread_spin_lock( &m_spinIterator );
	vecData.push_back( data );
	m_FileWrite << data.ToString( uPrecision ) << std::endl;
	pthread_cleanup_pop( 1 );
}

bool CGazeCapture_Set::WriteHeader( const std::string &sFile ) const
{
	std::fstream file( sFile, std::fstream::out );
	if( !file.is_open( ) )
	{
		fprintf( stderr, "Unable to open file \"%s\" for writing\n", sFile.c_str( ) );
		return false;
	}

	file << "name=" << sName << "\n";
	file << "dist=" << std::to_string( dEyeDistance * 100 ) << "cm\n";
	file << "data=" << sDataPath << "\n";
	file << "\ndata:\n";

	file.close( );
	return true;
}

bool CGazeCapture_Set::WriteAll( const std::string &sFile ) const
{
	std::fstream file( sFile, std::fstream::out );
	if( !file.is_open( ) )
	{
		fprintf( stderr, "Unable to open file \"%s\" for writing\n", sFile.c_str( ) );
		return false;
	}

	file << "name=" << sName << "\n";
	file << "dist=" << std::to_string( dEyeDistance * 100 ) << "cm\n";
	file << "data=" << sDataPath << "\n";
	file << "\ndata:\n";
	for( auto data : vecData )
		file << data.ToString( ) << std::endl;

	file.close( );
	return true;
}

unsigned CGazeCapture_Set::CheckDuplicates( bool fRemove )
{
	std::vector<gazecapture>::iterator itDuplicates = std::unique( vecData.begin( ), vecData.end( ), [ ]( const gazecapture &dataA, const gazecapture &dataB )
	{
		return dataA.uImage == dataB.uImage;
	} );

	unsigned uDuplicates = (unsigned) std::distance( itDuplicates, vecData.end( ) );
	if( fRemove )
		vecData.erase( itDuplicates, vecData.end( ) );

	return uDuplicates;
}

//===========================================================================
// CGazeData_Set
//===========================================================================

CGazeData_Set::gazedata::gazedata( std::string sLine )
{
	std::smatch match;
	std::replace( sLine.begin( ), sLine.end( ), '\r', ' ' );
	std::regex_match( sLine, match, s_regex_line );
	if( !match.size( ) )
		throw 0;

	{
		struct tm timeinfo = { 0 };
		timeinfo.tm_sec = std::stoi( match[ 6 ].str( ) );
		timeinfo.tm_min = std::stoi( match[ 5 ].str( ) );
		timeinfo.tm_hour = std::stoi( match[ 4 ].str( ) );
		timeinfo.tm_mday = std::stoi( match[ 3 ].str( ) );
		timeinfo.tm_mon = std::stoi( match[ 2 ].str( ) ) - 1;
		timeinfo.tm_year = std::stoi( match[ 1 ].str( ) ) - 1900;
		time = mktime( &timeinfo );
	}

	uImage = std::stoul( match[ 7 ].str( ) );
	dFOV = std::stod( match[ 8 ].str( ) );

	vec2EyeLeft = CVector<2>( { std::stod( match[ 9 ].str( ) ), std::stod( match[ 10 ].str( ) ) } );
	vec2PYLeft = CVector<2>( { std::stod( match[ 11 ].str( ) ), std::stod( match[ 12 ].str( ) ) } );
	vec2EyeRight = CVector<2>( { std::stod( match[ 13 ].str( ) ), std::stod( match[ 14 ].str( ) ) } );
	vec2PYRight = CVector<2>( { std::stod( match[ 15 ].str( ) ), std::stod( match[ 16 ].str( ) ) } );
}

std::string CGazeData_Set::gazedata::ToString( unsigned int uPrecision ) const
{
	std::ostringstream out;
	out.setf( std::ios_base::fixed, std::ios_base::floatfield );
	out.precision( uPrecision );

	//Write date
	{
		struct tm timeinfo;
#ifdef _MSC_VER
		localtime_s( &timeinfo, &time );
#else
		localtime_r( &time, &timeinfo );
#endif
		char szDate[ 20 ];
		strftime( szDate, 20, "%F %T", &timeinfo ); //YYYY-MM-DD HH:MM:SS
		out << szDate;
	}

	out << " " << uImage << " " << dFOV;
	out << " (" << vec2EyeLeft[ 0 ] << ", " << vec2EyeLeft[ 1 ] << ")@(" << vec2PYLeft[ 0 ] << ", " << vec2PYLeft[ 1 ] << ")";
	out << " (" << vec2EyeRight[ 0 ] << ", " << vec2EyeRight[ 1 ] << ")@(" << vec2PYRight[ 0 ] << ", " << vec2PYRight[ 1 ] << ")";
	return out.str( );
}

std::string CGazeData_Set::gazedata::ToCSV( unsigned int uPrecision ) const
{
	std::ostringstream out;
	out.setf( std::ios_base::fixed, std::ios_base::floatfield );
	out.precision( uPrecision );

	out << vec2EyeLeft[ 0 ] << "," << vec2EyeLeft[ 1 ] << ",";
	out << vec2PYLeft[ 0 ] << "," << vec2PYLeft[ 1 ] << ",";
	out << vec2EyeRight[ 0 ] << "," << vec2EyeRight[ 1 ] << ",";
	out << vec2PYRight[ 0 ] << "," << vec2PYRight[ 1 ];
	return out.str( );
}

CGazeData_Set CGazeData_Set::LoadList( const std::string &sFile )
{
	std::vector<gazedata> vecData;
	std::string sName;
	double dEyeDistance;
	std::string sDataPath;
	std::string sRawPath;

	std::fstream file( sFile, std::fstream::in );
	if( !file.is_open( ) )
	{
		fprintf( stderr, "Unable to open file \"%s\"\n", sFile.c_str( ) );
		throw 0;
	}

	std::smatch match;
	unsigned char fFound = 0;
	std::string sLine;
	while( std::getline( file, sLine ) )
	{
		std::regex_match( sLine, match, s_regex_name );
		if( match.size( ) )
		{
			sName = match[ 1 ].str( );
			fFound |= 1;
			continue;
		}

		std::regex_match( sLine, match, s_regex_dist );
		if( match.size( ) )
		{
			dEyeDistance = std::stod( match[ 1 ].str( ) ) / 100;
			fFound |= 2;
			continue;
		}

		std::regex_match( sLine, match, s_regex_datapath );
		if( match.size( ) )
		{
			sDataPath = match[ 1 ].str( );
			fFound |= 4;
			continue;
		}

		std::regex_match( sLine, match, s_regex_rawpath );
		if( match.size( ) )
		{
			sRawPath = match[ 1 ].str( );
			fFound |= 8;
			continue;
		}

		std::regex_match( sLine, match, s_regex_data );
		if( match.size( ) )
		{
			fFound |= 16;
			break;	//Start of data
		}
	}

	if( fFound != 31 )
	{
		fprintf( stderr, "File \"%s\" is missing fields\n", sFile.c_str( ) );
		file.close( );
		throw 1;
	}

	while( std::getline( file, sLine ) )
	{
		try
		{
			vecData.emplace_back( sLine );
		}
		catch( int )
		{
			continue;
		}
	}
	file.close( );
	return CGazeData_Set( vecData, sName, dEyeDistance, sDataPath, sRawPath );
}

CGazeData_Set::CGazeData_Set( const std::vector<gazedata> &vecData, const std::string &sName, double dEyeDistance, const std::string &sDataPath, const std::string &sRawPath ) :
	vecData( vecData ),
	sName( sName ),
	dEyeDistance( dEyeDistance ),
	sDataPath( sDataPath ),
	sRawPath( sRawPath )
{
	pthread_spin_init( &m_spinIterator, PTHREAD_PROCESS_PRIVATE );
	ResetIterator( );
}

CGazeData_Set::gazedata *CGazeData_Set::GetNext( void )
{
	gazedata *pData = nullptr;
	pthread_cleanup_push( ( void( *)( void * ) ) pthread_spin_unlock, (void *) &m_spinIterator );
	pthread_spin_lock( &m_spinIterator );
	if( m_itData < vecData.end( ) )
	{
		pData = &( *m_itData );
		m_itData++;
	}
	pthread_cleanup_pop( 1 );

	return pData;
}

void CGazeData_Set::Write( const gazedata &data, unsigned uPrecision )
{
	pthread_cleanup_push( ( void( *)( void * ) ) pthread_spin_unlock, (void *) &m_spinIterator );
	pthread_spin_lock( &m_spinIterator );
	vecData.push_back( data );
	m_FileWrite << data.ToString( uPrecision ) << std::endl;
	pthread_cleanup_pop( 1 );
}

bool CGazeData_Set::WriteHeader( const std::string &sFile ) const
{
	std::fstream file( sFile, std::fstream::out );
	if( !file.is_open( ) )
	{
		fprintf( stderr, "Unable to open file \"%s\" for writing\n", sFile.c_str( ) );
		return false;
	}

	file << "name=" << sName << "\n";
	file << "dist=" << std::to_string( dEyeDistance * 100 ) << "cm\n";
	file << "data=" << sDataPath << "\n";
	file << "raw=" << sRawPath << "\n";
	file << "\ndata:\n";

	file.close( );
	return true;
}

bool CGazeData_Set::WriteAll( const std::string &sFile ) const
{
	std::fstream file( sFile, std::fstream::out );
	if( !file.is_open( ) )
	{
		fprintf( stderr, "Unable to open file \"%s\" for writing\n", sFile.c_str( ) );
		return false;
	}

	file << "name=" << sName << "\n";
	file << "dist=" << std::to_string( dEyeDistance * 100 ) << "cm\n";
	file << "data=" << sDataPath << "\n";
	file << "raw=" << sRawPath << "\n";
	file << "\ndata:\n";
	for( auto data : vecData )
		file << data.ToString( ) << std::endl;

	file.close( );
	return true;
}

unsigned CGazeData_Set::CheckDuplicates( bool fRemove )
{
	std::vector<gazedata>::iterator itDuplicates = std::unique( vecData.begin( ), vecData.end( ), [ ]( const gazedata &dataA, const gazedata &dataB )
	{
		return dataA.uImage == dataB.uImage;
	} );

	unsigned uDuplicates = (unsigned) std::distance( itDuplicates, vecData.end( ) );
	if( fRemove )
		vecData.erase( itDuplicates, vecData.end( ) );

	return uDuplicates;
}

bool CGazeData_Set::Export( const std::string &sPath )
{
	std::fstream fileLabels( sPath + "/labels.csv", std::fstream::out );
	if( !fileLabels.is_open( ) )
	{
		fprintf( stderr, "Unable to open file \"%s\"\n", ( sPath + "/labels.csv" ).c_str( ) );
		return false;
	}

	std::fstream fileData( sPath + "/data.txt", std::fstream::out );
	if( !fileData.is_open( ) )
	{
		fprintf( stderr, "Unable to open file \"%s\"\n", ( sPath + "/data.txt" ).c_str( ) );
		fileLabels.close( );
		return false;
	}

	for( auto &data : vecData )
	{
		fileLabels << data.ToCSV( ) << std::endl;
		fileData << "img_" << data.uImage << ".jpg 0" << std::endl;
	}

	fileLabels.close( );
	fileData.close( );
	return true;
}