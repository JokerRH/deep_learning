#include "Columbia.h"
#include "Render\Vector.h"
#include <windows.h>
#include <Shlwapi.h>
#include <vector>
#include <iostream>
#include <PathCch.h>
#include <regex>
#include <math.h>
#include <fstream>

#undef LoadImage
void FindFilesRecursively( const std::wstring &sDir, const std::wstring &sPattern, std::vector<std::wstring> &vecsFiles )
{
	const WCHAR *lpFolder = sDir.c_str( );
	const WCHAR *lpFilePattern = sPattern.c_str( );
	WCHAR szFullPattern[ MAX_PATH ];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile;

	//Process subdirectories
	PathCchCombine( szFullPattern, MAX_PATH, lpFolder, L"*" );
	hFindFile = FindFirstFile( szFullPattern, &FindFileData );
	if( hFindFile != INVALID_HANDLE_VALUE )
	{
		do
		{
			if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && FindFileData.cFileName[ 0 ] != '.' )
			{
				//Found subdirectory, process
				PathCchCombine( szFullPattern, MAX_PATH, lpFolder, FindFileData.cFileName );
				FindFilesRecursively( szFullPattern, lpFilePattern, vecsFiles );
			}
		} while( FindNextFile( hFindFile, &FindFileData ) );
		FindClose( hFindFile );
	}

	// Now we are going to look for the matching files
	PathCchCombine( szFullPattern, MAX_PATH, lpFolder, lpFilePattern );
	hFindFile = FindFirstFile( szFullPattern, &FindFileData );
	if( hFindFile != INVALID_HANDLE_VALUE )
	{
		do
		{
			if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
			{
				//Found file
				PathCchCombine( szFullPattern, MAX_PATH, lpFolder, FindFileData.cFileName );
				vecsFiles.push_back( szFullPattern );
			}
		} while( FindNextFile( hFindFile, &FindFileData ) );
		FindClose( hFindFile );
	}
}

std::vector<std::wstring> CColumbiaData::Load( const std::wstring &sPath, const std::wstring &sFile )
{
	std::vector<std::wstring> vecsFiles;
	FindFilesRecursively( sPath, L"*.jpg", vecsFiles );
	std::wcout << "Found " << vecsFiles.size( ) << " files" << std::endl;

	if( !sFile.empty( ) ) do
	{
		std::wfstream smFile( sFile );
		if( !smFile.is_open( ) )
		{
			std::wcerr << "Unable to open file \"" << sFile << "\"" << std::endl;
			break;
		}

		WCHAR *szPath = (WCHAR*) _alloca( ( sFile.size( ) + 1 ) * sizeof( WCHAR ) );
		memcpy( (void *) szPath, sFile.c_str( ), ( sFile.size( ) + 1 ) * sizeof( WCHAR ) );
		PathCchRemoveFileSpec( szPath, ( sFile.size( ) + 1 ) * sizeof( WCHAR ) );

		std::wstring sLine;
		std::vector<CData> vecData;
		while( std::getline( smFile, sLine ) )
		{
			try
			{
				vecData.emplace_back( sLine, std::wstring( szPath ), false );
			}
			catch( int i )
			{
				if( i == 27 )
				{
					smFile.close( );
					throw;
				}
			}
		}

		smFile.close( );

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
		atan( std::stod( match[ 2 ].str( ) ) ) / 2.5,
		atan( std::stod( match[ 1 ].str( ) ) ) / 2.5,
		-0.5
	} );

	if( !LoadImage( sImage, sWindow ) )
		throw 0;

	CVector<2> vec2EyeLeft( { (double) ptEyeLeft.x, (double) ptEyeLeft.y } );
	CVector<2> vec2EyeRight( { (double) ptEyeRight.x, (double) ptEyeRight.y } );
	const double dPixelIPD = ( vec2EyeLeft - vec2EyeRight ).Abs( );
	const double dDistFact = 0.068 / dPixelIPD;

	CVector<2> vec2Center( { rectFace.width / 2.0, rectFace.height / 2.0 } );
	vec2EyeLeft = ( vec2EyeLeft - vec2Center ) * dDistFact;
	vec2EyeRight -= ( vec2EyeRight - vec2Center ) * dDistFact;
	vec3EyeLeft = CVector<3>( { vec2EyeLeft[ 0 ], vec2EyeLeft[ 1 ], 2.0 } );
	vec3EyeRight = CVector<3>( { vec2EyeLeft[ 0 ], vec2EyeLeft[ 1 ], 2.0 } );
}