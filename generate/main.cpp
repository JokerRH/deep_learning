#include "Columbia.h"
#include "Custom.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <locale>
#include <opencv2\highgui.hpp>

std::wstring StrToWStr( const std::string &str )
{
	const std::ctype<wchar_t> &ctype = std::use_facet<std::ctype<wchar_t>>( std::locale( ) );
	std::vector<wchar_t> wideStringBuffer( str.length( ) );
	ctype.widen( str.data( ), str.data( ) + str.length( ), &wideStringBuffer[ 0 ] );
	return std::wstring( &wideStringBuffer[ 0 ], wideStringBuffer.size( ) );
}

int main( int argc, char **argv )
{
	if( argc >= 3 )
	{
		if( !_stricmp( argv[ 1 ], "show" ) )
		{
			std::vector<CData> vecData = CData::LoadData( StrToWStr( argv[ 2 ] ), 4 );
			std::wstring sPath = CData::GetPath( StrToWStr( argv[ 2 ] ) );
			std::wcout << "Loaded " << vecData.size( ) << " instances" << std::endl;

			try
			{
				cv::namedWindow( "Window", cv::WINDOW_FULLSCREEN );
				cv::setWindowProperty( "Window", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN );
				for( CData data : vecData )
				{
					data.LoadImage( sPath );
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
	}

	std::wcout << "Invalid parameters" << std::endl;
	system( "PAUSE" );
	return EXIT_SUCCESS;
}