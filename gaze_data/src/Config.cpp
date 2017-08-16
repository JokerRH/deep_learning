#include "Config.h"
#include "Utility.h"
#include <fstream>
#include <regex>

const std::regex CConfig::s_regexMonPos( R"a(\s*monpos\s*=\s*\(\s*([+-]?(?:\d+(?:\.\d+)?)|(?:\.\d+))(?:(\S)?m)?\s*,\s*([+-]?(?:\d+(?:\.\d+)?)|(?:\.\d+))(?:(\S)?m)?\s*,\s*([+-]?(?:\d+(?:\.\d+)?)|(?:\.\d+))(?:(\S)?m)?\s*\).*)a" );
const std::regex CConfig::s_regexMonDim( R"a(\s*mondim\s*=\s*([+-]?(?:\d+(?:\.\d+)?)|(?:\.\d+))(?:(\S)?m)?\s*x\s*([+-]?(?:\d+(?:\.\d+)?)|(?:\.\d+))(?:(\S)?m)?\s*x\s*([+-]?(?:\d+(?:\.\d+)?)|(?:\.\d+))(?:(\S)?m)?.*)a" );

CConfig::CConfig( const std::string &sFile )
{
	std::ifstream file( sFile );
	
	std::string sLine;
	std::smatch match;
	unsigned char fFound = 0;
	while( std::getline( file, sLine ) )
	{
		if( fFound == 3 )
			break;

		std::regex_match( sLine, match, s_regexMonPos );
		if( match.size( ) )
		{
			vec3MonitorPos = CVector<3>(
			{
				std::stod( match[ 1 ].str( ) ) * CUtility::GetFactor( match[ 2 ].str( ) ),
				std::stod( match[ 3 ].str( ) ) * CUtility::GetFactor( match[ 4 ].str( ) ),
				std::stod( match[ 5 ].str( ) ) * CUtility::GetFactor( match[ 6 ].str( ) )
			} );
			fFound |= 1;
			continue;
		}
		
		std::regex_match( sLine, match, s_regexMonDim );
		if( match.size( ) )
		{
			vec3MonitorDim = CVector<3>(
			{
				std::stod( match[ 1 ].str( ) ) * CUtility::GetFactor( match[ 2 ].str( ) ),
				std::stod( match[ 3 ].str( ) ) * CUtility::GetFactor( match[ 4 ].str( ) ),
				std::stod( match[ 5 ].str( ) ) * CUtility::GetFactor( match[ 6 ].str( ) )
			} );
			fFound |= 2;
			continue;
		}
	}

	file.close( );

	if( fFound != 3 )
	{
		fprintf( stderr, "Config file \"%s\" is missing fields\n", sFile.c_str( ) );
		throw( 0 );
	}
}

void CConfig::Swap( CConfig &other )
{
	vec3MonitorPos.Swap( other.vec3MonitorPos );
	vec3MonitorDim.Swap( other.vec3MonitorDim );
}

CConfig &CConfig::operator=( const CConfig &other )
{
	if( this != &other )
	{
		CConfig temp( other );
		Swap( temp );
	}
	return *this;
}