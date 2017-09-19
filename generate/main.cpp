#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>

#include "Columbia.h"
#include "Custom.h"
#include "GazeData.h"
#include "Detect.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <locale>
#include <Windows.h>
#include <Shlwapi.h>
#include <Pathcch.h>
#include <opencv2\highgui.hpp>

#undef LoadImage

std::wstring StrToWStr( const std::string &str )
{
	const std::ctype<wchar_t> &ctype = std::use_facet<std::ctype<wchar_t>>( std::locale( ) );
	std::vector<wchar_t> wideStringBuffer( str.length( ) );
	ctype.widen( str.data( ), str.data( ) + str.length( ), &wideStringBuffer[ 0 ] );
	return std::wstring( &wideStringBuffer[ 0 ], wideStringBuffer.size( ) );
}

bool CreateCVWindow( const std::string &sWindow )
{
	cv::namedWindow( sWindow, cv::WINDOW_FULLSCREEN );
	cv::moveWindow( sWindow, 0, 0 );
	cv::setWindowProperty( sWindow, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN );

	HWND hWindow = FindWindow( 0, StrToWStr( sWindow ).c_str( ) );
	if( !hWindow )
	{
		std::cerr << "Unable to get window handle for \"" << sWindow << "\"" << std::endl;
		return false;
	}

	SetWindowLong( hWindow, GWL_STYLE, GetWindowLong( hWindow, GWL_STYLE ) & ~( WS_CAPTION | WS_THICKFRAME ) );
	SetWindowLong( hWindow, GWL_EXSTYLE, GetWindowLong( hWindow, GWL_EXSTYLE ) & ~( WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE ) );
	MONITORINFO monitor_info;
	monitor_info.cbSize = sizeof( monitor_info );
	GetMonitorInfo( MonitorFromWindow( hWindow, MONITOR_DEFAULTTONEAREST ), &monitor_info );
	SetWindowPos( hWindow, NULL, monitor_info.rcMonitor.left, monitor_info.rcMonitor.top, monitor_info.rcMonitor.right - monitor_info.rcMonitor.left, monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED );
	return true;
}

int wmain( int argc, WCHAR **argv )
{
#if 0
	auto vecData = CData::LoadData( LR"a(D:\Users\Rainer\Source\Repos\deep_learning\generate\x64\columbia\columbia_old.txt)a" );
	std::wsmatch match;
	const std::wregex regImage( LR"a(\d+_2m_0P_([+-]?\d+)V_([+-]?\d+)H\.jpg)a" );
	CData::OpenWrite( LR"a(D:\Users\Rainer\Source\Repos\deep_learning\generate\x64\columbia\columbia.txt)a" );
	for( auto &data : vecData )
	{
		std::regex_match( data.sImage, match, regImage );
		data.vec3GazePoint = CVector<3>( {
			tan( std::stod( match[ 2 ].str( ) ) * M_PI / 180 ) * 2.5,
			tan( std::stod( match[ 1 ].str( ) ) * M_PI / 180 ) * 2.5,
			-0.5
		} );

		data.fWriteImage = false;
		data.matImage = cv::Mat( cv::Size( 1, 1 ), CV_8UC3, cv::Scalar::all( 255 ) );
		data.WriteAsync( );
	}
	CData::CloseWrite( );
	system( "PAUSE" );
	return 0;
#endif

#if 0
	auto vecRaw = CCustom::Load( LR"a(D:\Users\Rainer\Source\Repos\deep_learning\gaze_data\raw\rainer.txt)a" );
	auto vecData = CData::LoadData( LR"a(D:\Users\Rainer\Source\Repos\deep_learning\generate\x64\rainer\rainer_old.txt)a" );

	CData::OpenWrite( LR"a(D:\Users\Rainer\Source\Repos\deep_learning\generate\x64\rainer\rainer.txt)a" );
	for( auto &filedata : vecRaw )
	{
		auto it = std::find_if( vecData.begin( ), vecData.end( ), [ filedata ]( const CData &data )
		{
			return PathFindFileName( filedata.sImage.c_str( ) ) == data.sImage;
		} );

		if( it == vecData.end( ) )
			throw 0;

		CData data( *it );
		if( !data.LoadImage( ) )
			throw 1;

		CVector<3> vec3ScreenTL( { -0.25, -0.07, 0.57 } );
		CVector<3> vec3ScreenDim( { 0.475, -0.268, -0.03 } );

		CVector<2> vec2GazePoint( {
			( -data.vec3GazePoint[ 0 ] - vec3ScreenTL[ 0 ] ) / vec3ScreenDim[ 0 ],
			( data.vec3GazePoint[ 1 ] - vec3ScreenTL[ 1 ] ) / vec3ScreenDim[ 1 ]
		} );

		std::wcout << "Gaze point: " << vec2GazePoint << std::endl;

		data.vec3GazePoint = CVector<3>( {
			-vec3ScreenTL[ 0 ] - vec2GazePoint[ 0 ] * abs( vec3ScreenDim[ 0 ] ),
			vec3ScreenTL[ 1 ] - vec2GazePoint[ 1 ] * abs( vec3ScreenDim[ 1 ] ),
			vec3ScreenTL[ 2 ]
		} );

		CVector<2> vec2EyeLeft( { (double) data.rectFace.x + data.ptEyeLeft.x, (double) data.rectFace.y + data.ptEyeLeft.y } );
		CVector<2> vec2EyeRight( { (double) data.rectFace.x + data.ptEyeRight.x, (double) data.rectFace.y + data.ptEyeRight.y } );

		const double dIPDFrac = 0.066 / ( vec2EyeLeft - vec2EyeRight ).Abs( );
		const double dFocalLength = ( sqrt( data.matImage.cols * data.matImage.cols + data.matImage.rows * data.matImage.rows ) / 2 ) / tan( filedata.dFOV / ( 2 * 180 ) * M_PI );

		data.vec3EyeLeft = CVector<3>( {
			vec2EyeLeft[ 0 ] - data.matImage.cols / 2.0,
			-( vec2EyeLeft[ 1 ] - data.matImage.rows / 2.0 ),
			dFocalLength
		} ) * dIPDFrac;

		data.vec3EyeRight = CVector<3>( {
			vec2EyeRight[ 0 ] - data.matImage.cols / 2.0,
			-( vec2EyeRight[ 1 ] - data.matImage.rows / 2.0 ),
			dFocalLength
		} ) * dIPDFrac;

		data.fWriteImage = false;
		data.WriteAsync( );
	}
	CData::CloseWrite( );

	system( "PAUSE" );
	return 0;
#endif

	std::wstring sExecPath;
	{
		WCHAR szPath[ MAX_PATH ];
		if( !GetFullPathName( argv[ 0 ], MAX_PATH, szPath, nullptr ) )
		{
			std::wcerr << "Unable to get full path for executable folder" << std::endl;
			return EXIT_FAILURE;
		}

		PathRemoveFileSpec( szPath );
		sExecPath = std::wstring( szPath );
	}

	if( argc >= 3 )
	{
		if( !_wcsicmp( argv[ 1 ], L"show" ) )
		{
			std::vector<CData> vecData = CData::LoadData( argv[ 2 ] );
			std::wcout << "Loaded " << vecData.size( ) << " instances" << std::endl;

			if( !CreateCVWindow( "Window" ) )
			{
				system( "PAUSE" );
				return EXIT_FAILURE;
			}

			try
			{
				for( CData data : vecData )
				{
					data.LoadImage( );
					std::wcout << "Showing" << data.sImage << std::endl;
					data.Show( "Window" );
				}
			}
			catch( int i )
			{
				cv::destroyAllWindows( );

				if( i != 27 )
				{
					system( "PAUSE" );
					return EXIT_FAILURE;
				}
				return EXIT_SUCCESS;
			}

			cv::destroyAllWindows( );
			std::wcout << "Done." << std::endl;
			system( "PAUSE" );
			return EXIT_SUCCESS;
		}
	}
	if( argc >= 4 )
	{
		if( !_wcsicmp( argv[ 1 ], L"columbia" ) )
		{
			std::vector<std::wstring> vecsImages = CColumbiaData::Load( argv[ 2 ], argv[ 3 ] );

			CData::Init( sExecPath );
			try
			{
				if( !CData::OpenWrite( argv[ 3 ] ) )
				{
					system( "PAUSE" );
					return EXIT_FAILURE;
				}

				cv::namedWindow( "Window", cv::WINDOW_FULLSCREEN );
				cv::setWindowProperty( "Window", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN );
				for( const std::wstring &sImage : vecsImages )
				{
					try
					{
						CColumbiaData data( sImage, "Window" );
						data.WriteAsync( );
					}
					catch( int i )
					{
						if( i == 27 )
							throw;
					}
				}
			}
			catch( int i )
			{
				cv::destroyAllWindows( );
				CData::CloseWrite( );

				if( i != 27 )
				{
					system( "PAUSE" );
					return EXIT_FAILURE;
				}
				return EXIT_SUCCESS;
			}

			cv::destroyAllWindows( );
			CData::CloseWrite( );
			std::wcout << "Done." << std::endl;
			system( "PAUSE" );
			return EXIT_SUCCESS;
		}
		else if( !_wcsicmp( argv[ 1 ], L"custom" ) )
		{
			std::vector<CCustom::fileformat> vecImages = CCustom::Load( argv[ 2 ], argv[ 3 ] );

			CData::Init( sExecPath  );
			try
			{
				if( !CData::OpenWrite( argv[ 3 ] ) )
				{
					system( "PAUSE" );
					return EXIT_FAILURE;
				}

				cv::namedWindow( "Window", cv::WINDOW_FULLSCREEN );
				cv::setWindowProperty( "Window", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN );
				for( const CCustom::fileformat &image : vecImages )
				{
					try
					{
						CCustom data( image, "Window" );
						data.WriteAsync( );
					}
					catch( int i )
					{
						if( i == 27 )
							throw;
					}
				}
			}
			catch( int i )
			{
				cv::destroyAllWindows( );
				CData::CloseWrite( );

				if( i != 27 )
				{
					system( "PAUSE" );
					return EXIT_FAILURE;
				}
				return EXIT_SUCCESS;
			}

			cv::destroyAllWindows( );
			CData::CloseWrite( );
			std::wcout << "Done." << std::endl;
			system( "PAUSE" );
			return EXIT_SUCCESS;
		}
		else if( !_wcsicmp( argv[ 1 ], L"imp" ) )
		{
			std::vector<CData> vecData;
			for( unsigned u = 3; u < (unsigned) argc; u++ )
			{
				std::vector<CData> vecTemp = CData::LoadData( argv[ u ] );
				std::wcout << "Loaded " << vecTemp.size( ) << " instances from \"" << argv[ u ] << "\"" << std::endl;
				vecData.insert( vecData.end( ), vecTemp.begin( ), vecTemp.end( ) );
			}
			std::wcout << "Loaded " << vecData.size( ) << " instances\n" << std::endl;

			std::vector<CGazeData> vecImport = CGazeData::LoadData( argv[ 2 ] );
			std::wcout << "Imported " << vecImport.size( ) << " instances" << std::endl;

			if( !CreateCVWindow( "Window" ) )
			{
				system( "PAUSE" );
				return EXIT_FAILURE;
			}

			try
			{
				for( CGazeData import : vecImport )
				{
					CData data = import.MergeReference( vecData );
					if( !data.IsValid( ) )
						continue;

					import.Show( "Window", data );
				}
			}
			catch( int i )
			{
				cv::destroyAllWindows( );

				if( i != 27 )
				{
					system( "PAUSE" );
					return EXIT_FAILURE;
				}
				return EXIT_SUCCESS;
			}

			cv::destroyAllWindows( );
			std::wcout << "Done." << std::endl;
			system( "PAUSE" );
			return EXIT_SUCCESS;
		}
#ifdef WITH_CAFFE
		else if( !_wcsicmp( argv[ 1 ], L"run" ) )
		{
			double dFOV = std::wcstod( argv[ 3 ], nullptr );
			std::wstring sImage;
			{
				WCHAR szPath[ MAX_PATH ];
				if( !GetFullPathName( argv[ 2 ], MAX_PATH, szPath, nullptr ) )
				{
					std::wcerr << "Unable to get full path for image" << std::endl;
					return EXIT_FAILURE;
				}

				sImage = std::wstring( szPath );
			}
			cv::Mat matImage( cv::imread( std::string( sImage.begin( ), sImage.end( ) ) ) );
			if( matImage.empty( ) )
			{
				std::wcerr << "Unable to read image \"" << sImage << "\"" << std::endl;
				system( "PAUSE" );
				return EXIT_FAILURE;
			}

			if( !CDetect::Init( sExecPath ) )
			{
				system( "PAUSE" );
				return EXIT_FAILURE;
			}

			std::vector<cv::Rect> vecFaces = CDetect::GetFaces( matImage );
			if( !CreateCVWindow( "Window" ) )
			{
				system( "PAUSE" );
				CDetect::Terminate( );
				return EXIT_FAILURE;
			}

			std::vector<CData> vecData;
			if( argc >= 5 )
			{
				WCHAR szPath[ MAX_PATH ];
				for( unsigned u = 4; u < (unsigned) argc; u++ )
				{
					if( !GetFullPathName( argv[ u ], MAX_PATH, szPath, nullptr ) )
					{
						std::wcerr << "Unable to get full path for data" << std::endl;
						return EXIT_FAILURE;
					}

					std::vector<CData> vecTemp = CData::LoadData( szPath );
					std::wcout << "Loaded " << vecTemp.size( ) << " instances from \"" << szPath << "\"" << std::endl;
					vecData.insert( vecData.end( ), vecTemp.begin( ), vecTemp.end( ) );
				}
				std::wcout << "Loaded " << vecData.size( ) << " instances\n" << std::endl;
			}

			try
			{
				for( const cv::Rect &rectFace : vecFaces )
				{
					CDetect detected( matImage, rectFace, dFOV, sImage );

					std::vector<CData>::iterator it = std::find_if( vecData.begin( ), vecData.end( ), [ detected ]( const CData &data )
					{
						return detected.sImage == data.sImage;
					} );

					if( it != vecData.end( ) )
					{
						it->LoadImage( );
						detected.Show( "Window", *it );
					}
					else
					{
						if( argc >= 5 )
							std::wcout << "Reference for image \"" << detected.sImage << "\" not found" << std::endl;
						detected.Show( "Window" );
					}
				}
			}
			catch( int i )
			{
				cv::destroyAllWindows( );
				CDetect::Terminate( );

				if( i != 27 )
				{
					system( "PAUSE" );
					return EXIT_FAILURE;
				}
				return EXIT_SUCCESS;
			}

			cv::destroyAllWindows( );
			CDetect::Terminate( );

			std::wcout << "Done." << std::endl;
			system( "PAUSE" );
			return EXIT_SUCCESS;
		}
#endif
	}
	if( argc >= 5 )
	{
		if( !_wcsicmp( argv[ 1 ], L"exp" ) )
		{
			std::vector<CData> vecData;
			for( unsigned u = 4; u < (unsigned) argc; u++ )
			{
				std::vector<CData> vecTemp = CData::LoadData( argv[ u ] );
				std::wcout << "Loaded " << vecTemp.size( ) << " instances from \"" << argv[ u ] << "\"" << std::endl;
				vecData.insert( vecData.end( ), vecTemp.begin( ), vecTemp.end( ) );
			}
			std::wcout << "Loaded " << vecData.size( ) << " instances\n" << std::endl;
			if( !CGazeData::Export( vecData, argv[ 3 ], std::stoul( argv[ 2 ] ) ) )
			{
				system( "PAUSE" );
				return EXIT_FAILURE;
			}

			std::wcout << "Done." << std::endl;
			system( "PAUSE" );
			return EXIT_SUCCESS;
		}
	}

	std::wcout << "Invalid parameters" << std::endl;
	for( unsigned u = 1; u < (unsigned) argc; u++ )
		std::wcout << u << ": \"" << argv[ u ] << "\"" << std::endl;
	system( "PAUSE" );
	return EXIT_SUCCESS;
}