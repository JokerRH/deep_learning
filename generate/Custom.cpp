#include "Custom.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <Windows.h>
#include <Pathcch.h>
#include <Shlwapi.h>
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>

#undef LoadImage

std::vector<CCustom::fileformat> CCustom::Load( const std::wstring &sPath, const std::wstring &sFile )
{
	std::vector<CCustom::fileformat> vecImages;
	std::fstream smFile( std::string( sPath.begin( ), sPath.end( ) ), std::fstream::in );
	if( !smFile.is_open( ) )
	{
		std::wcerr << "Unable to open file \"" << sPath << "\"" << std::endl;
		return vecImages;
	}

	std::string sLine;
	//Find data path
	do
	{
		if( !std::getline( smFile, sLine ) )
		{
			std::wcerr << "File \"" << sPath << "\" is missing data segment" << std::endl;
			smFile.close( );
			return vecImages;
		}
	} while( !std::regex_match( sLine, std::regex( R"a(\s*data:.*)a" ) ) );

	const std::regex regLine( R"a((\d{4})-(\d{2})-(\d{2})\s+(\d{2}):(\d{2}):(\d{2})\s+(\d+)\s+((?:\d+(?:\.\d+)?)|(?:\.\d+))\s+\(((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+)),\s+((?:(?:\+|-|)\d+(?:\.\d+)?)|(?:(?:\+|-|)\.\d+))\).*)a" );
	while( std::getline( smFile, sLine ) )
	{
		std::smatch match;
		std::replace( sLine.begin( ), sLine.end( ), '\r', ' ' );
		std::regex_match( sLine, match, regLine );
		if( !match.size( ) )
			continue;

		fileformat data;
		data.sImage = LR"a(Path\img_)a" + std::to_wstring( std::stoul( match[ 7 ].str( ) ) ) + L".jpg";
		data.dFOV = std::stod( match[ 8 ].str( ) );
		data.vec3GazePoint = CVector<3>( { std::stod( match[ 9 ].str( ) ), std::stod( match[ 10 ].str( ) ), std::stod( match[ 11 ].str( ) ) } );
		vecImages.push_back( data );
	}

	std::wcout << "Found " << vecImages.size( ) << " images" << std::endl;

	if( !sFile.empty( ) ) do
	{
		std::vector<CData> vecData = CData::LoadData( sFile );

		//Remove processed
		vecImages.erase( std::remove_if( vecImages.begin( ), vecImages.end( ), [ vecData ]( const fileformat &filedata )
		{
			return std::find_if( vecData.begin( ), vecData.end( ), [ filedata ]( const CData &data )
			{
				return PathFindFileName( filedata.sImage.c_str( ) ) == data.sImage;
			} ) != vecData.end( );
		} ), vecImages.end( ) );

		std::wcout << "Already processed: " << vecData.size( ) << std::endl;
		std::wcout << "Remaining: " << vecImages.size( ) << std::endl;
	} while( 0 );

	smFile.close( );
	return vecImages;
}

CCustom::CCustom( const fileformat &data, const std::string & sWindow )
{
	vec3GazePoint = data.vec3GazePoint;
	
	if( !LoadImage( data.sImage, sWindow ) )
		throw 0;

	CVector<2> vec2EyeLeft( { (double) ( rectFace.x + ptEyeLeft.x ), (double) ( rectFace.y + ptEyeLeft.y ) } );
	CVector<2> vec2EyeRight( { (double) ( rectFace.x + ptEyeRight.x ), (double) ( rectFace.y + ptEyeRight.y ) } );

	const double dIPDFrac = 0.066 / ( vec2EyeLeft - vec2EyeRight ).Abs( );
	const double dFocalLength = ( sqrt( matImage.cols * matImage.cols + matImage.rows * matImage.rows ) / 2 ) / tan( data.dFOV / ( 2 * 180 ) * M_PI );

	vec3EyeLeft = CVector<3>( {
		vec2EyeLeft[ 0 ] - matImage.cols / 2.0,
		-( vec2EyeLeft[ 1 ] - matImage.rows / 2.0 ),
		dFocalLength
	} ) * dIPDFrac;

	vec3EyeRight = CVector<3>( {
		vec2EyeRight[ 0 ] - matImage.cols / 2.0,
		-( vec2EyeRight[ 1 ] - matImage.rows / 2.0 ),
		dFocalLength
	} ) * dIPDFrac;
}