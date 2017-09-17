#include "Columbia.h"
#include "Render\Vector.h"
#include <windows.h>
#include <Shlwapi.h>
#include <vector>
#include <iostream>
#include <PathCch.h>
#include <regex>
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <fstream>

#undef LoadImage

std::vector<std::wstring> CColumbiaData::Load( const std::wstring &sPath, const std::wstring &sFile )
{
	std::vector<std::wstring> vecsFiles;
	FindFilesRecursively( sPath, L"*.jpg", vecsFiles );
	std::wcout << "Found " << vecsFiles.size( ) << " files" << std::endl;

	if( !sFile.empty( ) ) do
	{
		std::vector<CData> vecData = LoadData( sFile );

		//Remove processed
		vecsFiles.erase( std::remove_if( vecsFiles.begin( ), vecsFiles.end( ), [vecData]( const std::wstring &sPath )
			{
				return std::find_if( vecData.begin( ), vecData.end( ), [ sPath ]( const CData &data )
					{
						return PathFindFileName( sPath.c_str( ) ) == data.sImage;
					} ) != vecData.end( );
			} ), vecsFiles.end( ) );

		std::wcout << "Already processed: " << vecData.size( ) << std::endl;
		std::wcout << "Remaining: " << vecsFiles.size( ) << std::endl;
	} while( 0 );

	std::vector<std::wstring> vecsSelected;
	std::wsmatch match;
	const std::wregex regImage( LR"a(\d+_2m_0P_([+-]?\d+)V_([+-]?\d+)H\.jpg)a" );
	for( const std::wstring &sFile : vecsFiles )
	{
		std::wstring sName = PathFindFileName( sFile.c_str( ) );
		std::regex_match( sName, match, regImage );
		if( !match.size( ) )
			continue;

		vecsSelected.push_back( sFile );
	}

	return vecsSelected;
}

CColumbiaData::CColumbiaData( const std::wstring &sImage, const std::string &sWindow )
{
	std::wsmatch match;
	const std::wregex regImage( LR"a(\d+_2m_0P_([+-]?\d+)V_([+-]?\d+)H\.jpg)a" );
	std::wstring sName( PathFindFileName( sImage.c_str( ) ) );
	std::regex_match( sName, match, regImage );
	if( !match.size( ) )
		throw 0;

	vec3GazePoint = CVector<3>( {
		tan( std::stod( match[ 2 ].str( ) ) * M_PI / 180 ) * 2.5,
		tan( std::stod( match[ 1 ].str( ) ) * M_PI / 180 ) * 2.5,
		-0.5
	} );

	if( !LoadImage( sImage, sWindow ) )
		throw 0;

	CVector<2> vec2EyeLeft( { (double) rectFace.x + ptEyeLeft.x, (double) rectFace.y + ptEyeLeft.y } );
	CVector<2> vec2EyeRight( { (double) rectFace.x + ptEyeRight.x, (double) rectFace.y + ptEyeRight.y } );
	const double dPixelIPD = ( vec2EyeLeft - vec2EyeRight ).Abs( );
	const double dDistFact = 0.068 / dPixelIPD;

	CVector<2> vec2Center( { matImage.cols / 2.0, matImage.rows / 2.0 } );
	vec2EyeLeft = ( vec2EyeLeft - vec2Center ) * dDistFact;
	vec2EyeRight = ( vec2EyeRight - vec2Center ) * dDistFact;
	vec2EyeLeft[ 1 ] = -vec2EyeLeft[ 1 ];
	vec2EyeRight[ 1 ] = -vec2EyeRight[ 1 ];
	vec3EyeLeft = CVector<3>( { vec2EyeLeft[ 0 ], vec2EyeLeft[ 1 ], 2.0 } );
	vec3EyeRight = CVector<3>( { vec2EyeRight[ 0 ], vec2EyeRight[ 1 ], 2.0 } );
}