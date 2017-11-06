#include "Display.h"
#include "Detect.h"
#include <iostream>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>

cv::Rect CDisplay::Show( const std::string & sWindow, const cv::Mat & matImage )
{
	cv::Mat mat( GetScreenResolution( ), CV_8UC3, cv::Scalar::all( 255 ) );

	double dScale = mat.cols / (double) matImage.cols;
	if( matImage.rows * dScale > (double) mat.rows )
		dScale = mat.rows / (double) matImage.rows;

	cv::Size size( (int) ( matImage.cols * dScale ), (int) ( matImage.rows * dScale ) );
	cv::Rect rect( ( mat.cols - size.width ) / 2, ( mat.rows - size.height ) / 2, size.width, size.height );
	cv::resize( matImage, mat( rect ), size );

	cv::imshow( sWindow, mat );
	return rect;
}

void CDisplay::ShowImage( const std::string &sWindow, const CData &data, const CData &dataref )
{
	CDisplay display( data );
	display.DrawFace( data );

	display.m_Scenery.Fit( ).Draw( display.GetSceneryROI( ) );
	display.Show( sWindow );

	bool fReference = false;

	MSG msg;
	BOOL fReturn;
	while( true )
	{
		if( ( fReturn = GetMessage( &msg, NULL, 0, 0 ) ) == -1 )
			throw 27;	//Error

		switch( msg.message )
		{
		case WM_QUIT:
			std::wcout << "Quit message" << std::endl;
			PostQuitMessage( 0 );
			throw 27;
		case WM_KEYDOWN:
			{
				unsigned uKey = MapVirtualKey( (UINT) msg.wParam, MAPVK_VK_TO_CHAR );
				if( !uKey )
				{
					uKey = MapVirtualKey( (UINT) msg.wParam, MAPVK_VK_TO_VSC );
					if( ProcessKey( display.m_Scenery, { fReference ? &dataref : &data }, uKey, true ) )
					{
						display.m_Scenery.Draw( display.GetSceneryROI( ) );
						display.Show( sWindow );
					}

					break;
				}

				if( ProcessKey( display.m_Scenery, { fReference ? &dataref : &data }, uKey, false ) )
				{
					display.m_Scenery.Draw( display.GetSceneryROI( ) );
					display.Show( sWindow );
				}
				switch( uKey )
				{
				case 9:		//Tab
					if( !dataref.IsValid( ) )
						break;

					if( fReference )
					{
						display.m_matScreen = cv::Scalar::all( 255 );
						display.m_Scenery = display.m_Scenery.GetTransformation( ) * CScenery( data );
						display.DrawFace( data );
					}
					else
					{
						display.m_Scenery = display.m_Scenery.GetTransformation( ) * CScenery( dataref );
						display.DrawFace( dataref );
						putText( display.m_matScreen, display.m_sReference, display.m_ptText, display.m_iFontFace, display.m_dFontScale, cv::Scalar( 51, 153, 255 ), display.m_iFontThickness, 8 );
					}

					fReference = !fReference;
					display.m_Scenery.Draw( display.GetSceneryROI( ) );
					display.Show( sWindow );
					break;
				case 13:	//Enter
					return;
				case 27:	//Escape
					throw 27;
				}
				break;
			}
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			if( display.ProcessMouse( display.m_Scenery, msg ) )
			{
				display.m_Scenery.Draw( display.GetSceneryROI( ) );
				display.Show( sWindow );
			}
			break;
		}

		if( fReturn > 0 )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
}

#ifdef WITH_CAFFE
void CDisplay::ShowLive( const std::string & sWindow, CCamera &camera )
{
	camera.WaitForLiveView( );
	cv::Mat matImage;
	double dFOV;
	camera.TakePicture( matImage, dFOV );	//Get FOV value

	CDisplay display( (double) matImage.cols / matImage.rows );

	MSG msg;
	bool fUpdate = true;
	while( true )
	{
		if( !PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) && fUpdate )
		{
			//No message available, update image
			if( !camera.DownloadLiveView( matImage ) )
				continue;

			display.SetData( matImage, dFOV );
			display.Show( sWindow );
			continue;
		}

		switch( msg.message )
		{
		case WM_QUIT:
			std::wcout << "Quit message" << std::endl;
			PostQuitMessage( 0 );
			throw 27;
		case WM_KEYDOWN:
			{
				unsigned uKey = MapVirtualKey( (UINT) msg.wParam, MAPVK_VK_TO_CHAR );
				if( !uKey )
				{
					uKey = MapVirtualKey( (UINT) msg.wParam, MAPVK_VK_TO_VSC );
					if( ProcessKey( display.m_Scenery, display.m_vecData.begin( )._Ptr, display.m_vecData.end( )._Ptr, uKey, true ) )
					{
						display.m_Scenery.Draw( display.GetSceneryROI( ) );
						display.Show( sWindow );
					}

					break;
				}

				if( ProcessKey( display.m_Scenery, display.m_vecData.begin( )._Ptr, display.m_vecData.end( )._Ptr, uKey, false ) )
				{
					display.m_Scenery.Draw( display.GetSceneryROI( ) );
					display.Show( sWindow );
				}
				switch( uKey )
				{
				case 13:	//Enter
					if( fUpdate )
						return;

					fUpdate = true;
					break;
				case 27:	//Escape
					throw 27;
				case 80:	//'p'
					camera.TakePicture( );
					break;
					//default:
					//std::wcout << "Key: " << uKey << std::endl;
				}
				break;
			}
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			if( display.ProcessMouse( display.m_Scenery, msg ) )
			{
				display.m_Scenery.Draw( display.GetSceneryROI( ) );
				display.Show( sWindow );
			}
			break;
		case CAMERA_IMAGE_READY:
			if( !( (CCamera *) msg.wParam )->DownloadImage( matImage, dFOV, (void *) msg.lParam ) )
				break;

			display.SetData( matImage, dFOV );
			display.Show( sWindow );
			fUpdate = false;
			break;
		}

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}
#endif

CDisplay::CDisplay( double dRatio ) :
	m_matScreen( GetScreenResolution( ), CV_8UC3, cv::Scalar::all( 255 ) ),
	m_Scenery{ }
{
	cv::Size sizeImage( m_matScreen.rows >> 1, (unsigned) ( ( m_matScreen.rows >> 1 ) / dRatio ) );
	cv::Size sizeScenery( m_matScreen.rows, m_matScreen.rows );

	int iBaseLine = 0;
	cv::Size sizeText( cv::getTextSize( m_sReference, m_iFontFace, m_dFontScale, m_iFontThickness, &iBaseLine ) );

	cv::Size sizeTotal( sizeScenery.width + std::max( sizeImage.width, sizeText.width ), std::max( sizeScenery.height, sizeImage.height + sizeText.height ) );
	double dScale = 1;
	dScale = std::min( dScale, (double) m_matScreen.cols / sizeTotal.width );
	dScale = std::min( dScale, (double) m_matScreen.rows / sizeTotal.height );

	sizeImage = cv::Size( (unsigned) ( sizeImage.width * dScale ), (unsigned) ( sizeImage.height * dScale ) );
	sizeScenery = cv::Size( (unsigned) ( sizeScenery.width * dScale ), (unsigned) ( sizeScenery.height * dScale ) );
	sizeTotal = cv::Size( (unsigned) ( sizeTotal.width * dScale ), (unsigned) ( sizeTotal.height * dScale ) );
	m_dFontScale *= dScale;

	m_rectTotal = cv::Rect( (int) ( m_matScreen.cols / 2.0 - sizeTotal.width / 2.0 ), (int) ( m_matScreen.rows / 2.0 - sizeTotal.height / 2.0 ), sizeTotal.width, sizeTotal.height );
	m_rectScenery = cv::Rect( m_rectTotal.tl( ), sizeScenery );
	m_rectImage = cv::Rect( m_rectScenery.tl( ) + cv::Point( sizeScenery.width, 0 ), sizeImage );
	m_ptText = cv::Point( m_rectImage.tl( ) + cv::Point( 0, sizeImage.height + sizeText.height ) );
}

CDisplay::CDisplay( const CData &data ) :
	CDisplay( (double) data.matImage.cols / data.matImage.rows )
{
	m_vecData.emplace_back( data );
	m_Scenery = CScenery{ &data };
}

#ifdef WITH_CAFFE
void CDisplay::SetData( const cv::Mat &matImage, double dFOV )
{
	std::vector<cv::Rect> vecFaces = CDetect::GetFaces( matImage );

	cv::Mat matDraw;
	double dScaleX = (double) m_rectImage.width / matImage.cols;
	double dScaleY = (double) m_rectImage.height / matImage.rows;
	cv::resize( matImage, matDraw, m_rectImage.size( ) );

	m_vecData.clear( );
	for( const cv::Rect &face : vecFaces )
	{
		CDetect data( matImage, face, dFOV );
		m_vecData.emplace_back( data );

		cv::Rect rectFace( (int) ( data.rectFace.x * dScaleX ), (int) ( data.rectFace.y * dScaleY ), (int) ( data.rectFace.width * dScaleX ), (int) ( data.rectFace.height * dScaleY ) );
		cv::Point ptEyeLeft( (int) ( data.ptEyeLeft.x * dScaleX ), (int) ( data.ptEyeLeft.y * dScaleY ) );
		cv::Point ptEyeRight( (int) ( data.ptEyeRight.x * dScaleX ), (int) ( data.ptEyeRight.y * dScaleY ) );
		cv::rectangle( matDraw, rectFace, cv::Scalar( 0, 0, 255 ), 2 );
		cv::circle( matDraw, ptEyeLeft + rectFace.tl( ), 1, cv::Scalar( 0, 255, 0 ), -1 );
		cv::circle( matDraw, ptEyeRight + rectFace.tl( ), 1, cv::Scalar( 255, 0, 0 ), -1 );
	}

	matDraw.copyTo( m_matScreen( m_rectImage ) );
	m_Scenery = m_Scenery.GetTransformation( ) * CScenery( m_vecData.data( ), m_vecData.data( ) + m_vecData.size( ) );
	m_Scenery.Draw( GetSceneryROI( ) );
}
#endif

cv::Size CDisplay::GetScreenResolution( void )
{
	RECT desktop;
	GetWindowRect( GetDesktopWindow( ), &desktop );
	return cv::Size( desktop.right, desktop.bottom );
}

void CDisplay::DrawFace( const CData &data )
{
	cv::Mat matImage = data.matImage.clone( );
	double dScaleX = (double) m_rectImage.width / data.matImage.cols;
	double dScaleY = (double) m_rectImage.height / data.matImage.rows;
	cv::resize( matImage, matImage, m_rectImage.size( ) );
	cv::Rect rectFace( (int) ( data.rectFace.x * dScaleX ), (int) ( data.rectFace.y * dScaleY ), (int) ( data.rectFace.width * dScaleX ), (int) ( data.rectFace.height * dScaleY ) );
	cv::Point ptEyeLeft( (int) ( data.ptEyeLeft.x * dScaleX ), (int) ( data.ptEyeLeft.y * dScaleY ) );
	cv::Point ptEyeRight( (int) ( data.ptEyeRight.x * dScaleX ), (int) ( data.ptEyeRight.y * dScaleY ) );
	cv::rectangle( matImage, rectFace, cv::Scalar( 0, 0, 255 ), 2 );
	cv::circle( matImage, ptEyeLeft + rectFace.tl( ), 1, cv::Scalar( 0, 255, 0 ), -1 );
	cv::circle( matImage, ptEyeRight + rectFace.tl( ), 1, cv::Scalar( 255, 0, 0 ), -1 );
	matImage.copyTo( m_matScreen( m_rectImage ) );
}