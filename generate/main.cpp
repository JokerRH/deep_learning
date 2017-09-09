#include "Columbia.h"
#include "Custom.h"
#include "GazeData.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <locale>
#include <Windows.h>
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

#include "Render\Matrix.h"
int main( int argc, char **argv )
{
	if( argc >= 3 )
	{
		if( !_stricmp( argv[ 1 ], "show" ) )
		{
			std::vector<CData> vecData = CData::LoadData( StrToWStr( argv[ 2 ] ), 4 );
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
		if( !_stricmp( argv[ 1 ], "columbia" ) )
		{
			std::vector<std::wstring> vecsImages = CColumbiaData::Load( StrToWStr( argv[ 2 ] ), StrToWStr( argv[ 3 ] ) );

			CData::Init( );
			try
			{
				if( !CData::OpenWrite( StrToWStr( argv[ 3 ] ) ) )
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
		else if( !_stricmp( argv[ 1 ], "custom" ) )
		{
			std::vector<CCustom::fileformat> vecImages = CCustom::Load( StrToWStr( argv[ 2 ] ), StrToWStr( argv[ 3 ] ) );

			CData::Init( );
			try
			{
				if( !CData::OpenWrite( StrToWStr( argv[ 3 ] ) ) )
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
		else if( !_stricmp( argv[ 1 ], "imp" ) )
		{
			std::vector<CData> vecData;
			for( unsigned u = 3; u < (unsigned) argc; u++ )
			{
				std::vector<CData> vecTemp = CData::LoadData( StrToWStr( argv[ u ] ) );
				std::wcout << "Loaded " << vecTemp.size( ) << " instances from \"" << argv[ u ] << "\"" << std::endl;
				vecData.insert( vecData.end( ), vecTemp.begin( ), vecTemp.end( ) );
			}
			std::wcout << "Loaded " << vecData.size( ) << " instances\n" << std::endl;

			std::vector<CGazeData> vecImport = CGazeData::LoadData( StrToWStr( argv[ 2 ] ) );
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

					import.Show( "Window" );
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
	if( argc >= 5 )
	{
		if( !_stricmp( argv[ 1 ], "exp" ) )
		{
			std::vector<CData> vecData;
			for( unsigned u = 4; u < (unsigned) argc; u++ )
			{
				std::vector<CData> vecTemp = CData::LoadData( StrToWStr( argv[ u ] ) );
				std::wcout << "Loaded " << vecTemp.size( ) << " instances from \"" << argv[ u ] << "\"" << std::endl;
				vecData.insert( vecData.end( ), vecTemp.begin( ), vecTemp.end( ) );
			}
			std::wcout << "Loaded " << vecData.size( ) << " instances\n" << std::endl;
			if( !CGazeData::Export( vecData, StrToWStr( argv[ 3 ] ), std::stoul( argv[ 2 ] ) ) )
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
	system( "PAUSE" );
	return EXIT_SUCCESS;
}