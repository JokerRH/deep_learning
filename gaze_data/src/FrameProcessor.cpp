#include "FrameProcessor.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <deque>

using namespace cv;

CFrameProcessor::CFrameProcessor( void ) :
	m_Image( "Image_Main", m_matImage )
{
	if( !m_FaceCascade.load( "./haarcascades/haarcascade_frontalface_alt.xml" ) )
	{
		fprintf( stderr, "Unable to load face cascade\n" );
		throw 0;
	}

	if( !m_EyeCascade.load( "./haarcascades/haarcascade_eye.xml" ) )
	{
		fprintf( stderr, "Unable to load eye cascade\n" );
		throw 0;
	}
}

CFrameProcessor::~CFrameProcessor( void )
{

}

void CFrameProcessor::ProcessImage( void )
{
	unsigned int uFaces;
	
	Mat matDraw;
	CImage imgDraw( m_Image, "Image_Draw", matDraw );

	landmark_face *aFaces = GetFaces( uFaces );
	if( !aFaces && uFaces )
		return;

	for( unsigned int i, u = 0; u < uFaces; u++ )
	{
		aFaces[ u ].boxFace.TransferOwnership( imgDraw );	//m_Image -> imgDraw
		aFaces[ u ].boxFace.Draw( Scalar( 0, 0, 255 ), 2, 0 );
		for( i = 0; i < aFaces[ u ].vecEyes.size( ); i++ )
			aFaces[ u ].vecEyes[ i ].Draw( Scalar( 255, 0, 0 ), 2, 0 );

		aFaces[ u ].~landmark_face( );
	}

	free( aFaces );
	imgDraw.Show( "Window" );
}

landmark_face *CFrameProcessor::GetFaces( unsigned int &uFaces )
{
	Mat matGray;
	CImage imgGray( m_Image, "Image_Gray", matGray );
	cvtColor( matGray, matGray, CV_BGR2GRAY );
	equalizeHist( matGray, matGray );

	landmark_face *aFaces = DetectFaces( imgGray, uFaces );
	if( !aFaces )
		return nullptr;

	for( unsigned int u = 0; u < uFaces; u++ )
		aFaces[ u ].boxFace.TransferOwnership( );	//imgGray -> m_Image
	
	//m_Image
	//->aFaces[ x ].boxFace
	//  ->aFaces[ x ].vecEyes[ y ]
	return aFaces;
}

face_landmark *CFrameProcessor::ProcessImage( unsigned int &uLandmarks )
{
	landmark_face *aFaces = GetFaces( uLandmarks );
	if( !aFaces )
		return nullptr;

	face_landmark *aLandmarks = (face_landmark *) malloc( uLandmarks * sizeof( face_landmark ) );
	if( !aLandmarks )
	{
		fprintf( stderr, "Error allocating memory for landmarks\n" );
		return nullptr;
	}

	Mat matFocus;
	CImage imgFocus( "Image_Focus", matFocus );
	Mat matDraw;
	unsigned char c;
	bool fContinue;
	unsigned int uFaces = uLandmarks;
	uLandmarks = -1;
	CImage imgDraw;
	for( unsigned int i = 0; i < uFaces; i++ )
	{
		//Crop the face and draw a box around the eyes
		//m_Image
		//->aFaces[ x ].boxFace
		//  ->imgFocus (Face)
		//  ->aFaces[ x ].vecEyes[ y ]
		imgFocus.Crop( aFaces[ i ].boxFace, -1 );

		std::deque<CBBox> aEyes( aFaces[ i ].vecEyes.begin( ), aFaces[ i ].vecEyes.end( ) );
		fContinue = true;
		while( fContinue )
		{
			imgDraw = CImage( imgFocus, "Image_Draw", matDraw );
			std::deque<CBBox>::iterator it = aEyes.begin( );
			it->Draw( imgDraw, Scalar( 0, 255, 0 ) );
			it++;
			for( ; it < aEyes.end( ); it++ )
				it->Draw( imgDraw, Scalar( 255, 0, 0 ) );
			imgDraw.Show( "Window" );

			c = (unsigned char) waitKey( 0 );
			switch( c )
			{
			case 8:		//Backspace
				c = 0;
				fContinue = false;
				break;
			case 9:		//Tab
				aEyes.push_back( aEyes.front( ) );
				aEyes.pop_front( );
				break;
			case 10:	//Enter
				uLandmarks++;
				new( aLandmarks + uLandmarks ) face_landmark( aFaces[ i ].boxFace );
				fContinue = false;
				break;
			case 27:	//Escape
				goto RETURN;
			default:
				printf( "Key: %u\n", c & 0xFF );
			}
		}

		if( !c )
			continue;	//Ignore

		for( std::deque<CBBox>::iterator it = aEyes.begin( ); it < aEyes.end( ); )
		{
			//m_Image
			//->aFaces[ x ].boxFace
			//  ->aFaces[ x ].vecEyes[ y ]
			//    ->imgFocus (Eye)
			//      ->pt
			imgFocus.Crop( *it, -1 );
			CPoint pt( imgFocus, 0.5, 0.5 );
			unsigned int uX = pt.GetPositionX( 0 );
			unsigned int uY = pt.GetPositionY( 0 );

			fContinue = true;
			while( fContinue )
			{
				imgDraw = CImage( imgFocus, "Image_Draw", matDraw );
				pt = CPoint( imgDraw, cv::Point( uX, uY ) );
				pt.Draw( Scalar( 0, 255, 255 ), 1, -1, 0 );
				imgDraw.Show( "Window" );

				c = (unsigned char) waitKey( 0 );
				switch( c )
				{
				case 81:	//Key_Left
					if( uX )
						uX--;
					break;
				case 82:	//Key_Up
					if( uY )
						uY--;
					break;
				case 83:	//Key_Right
					if( uX < pt.GetParent( 1 )->GetWidth( 0 ) )
						uX++;
					break;
				case 84:	//Key_Down
					if( uY < pt.GetParent( 1 )->GetHeight( 0 ) )
						uY++;
					break;
				case 10:	//Enter
					//m_Image
					//->aLandmarks[ uLandmarks ].boxFace
					//->aFaces[ x ].boxFace
					//  ->aFaces[ x ].vecEyes[ y ]
					//    ->imgFocus (Eye)
					//      ->imgDraw(Eye)
					//        ->pt
					fContinue = false;
					if( it == aEyes.begin( ) )
					{
						//First in list, left eye
						aLandmarks[ uLandmarks ].ptEyeLeft = pt;
						aLandmarks[ uLandmarks ].ptEyeLeft.TransferOwnership( aLandmarks[ uLandmarks ].boxFace );
					}
					else
					{
						//Right eye
						aLandmarks[ uLandmarks ].ptEyeRight = pt;
						aLandmarks[ uLandmarks ].ptEyeRight.TransferOwnership( aLandmarks[ uLandmarks ].boxFace );
					}
					it++;
					break;
				case 8:		//Backspace
					fContinue = false;
					aEyes.erase( it );
					break;
				}
			}
		}

		//Draw face and eyes to check
		//m_Image
		//->aLandmarks[ x ].boxFace
		//  ->aLandmarks[ x ].ptEyeLeft
		//  ->aLandmarks[ x ].ptEyeRight
		CImage imgDraw( m_Image, "Image_Draw", matDraw );
		aLandmarks[ uLandmarks ].Draw( imgDraw );
		//aEyes[ 0 ].Draw( imgDraw, Scalar( 0, 255, 0 ) );
		//aEyes[ 1 ].Draw( imgDraw, Scalar( 255, 0, 0 ) );
		imgDraw.Show( "Window" );
		fContinue = true;
		while( fContinue )
		{
			c = (unsigned char) waitKey( 0 );
			switch( c )
			{
			case 8:		//Backspace
				i--;
				aLandmarks[ uLandmarks-- ].~face_landmark( );
			case 10:	//Enter
				fContinue = false;
				break;
			}
		}
	}

RETURN:
	for( unsigned int i = 0; i < uFaces; i++ )
		aFaces[ i ].~landmark_face( );

	free( aFaces );
	uLandmarks++;
	return aLandmarks;
}

landmark_face *CFrameProcessor::DetectFaces( CImage &imgGray, unsigned int &uFaces )
{
	std::vector<Rect> vecFaces;
	m_FaceCascade.detectMultiScale( *imgGray.pmatImage, vecFaces, 1.1, 5, CV_HAAR_SCALE_IMAGE, Size( 30, 30 ) );

	uFaces = vecFaces.size( );
	if( !uFaces )
		return nullptr;

	landmark_face *aFaces = (landmark_face *) malloc( sizeof( landmark_face ) * uFaces );
	if( !aFaces )
	{
		fprintf( stderr, "Error allocating memory for face bbox array" );
		return nullptr;
	}

	Mat matFace;
	CImage imgFace( "Image_Face", matFace );
	std::vector<Rect> vecEyes;
	for( unsigned int j, i = 0; i < uFaces; i++ )
	{
		new( aFaces + i ) landmark_face( CBBox( imgGray, vecFaces[ i ], "BBox_Face" ) );
		imgFace.Crop( aFaces[ i ].boxFace );

		vecEyes.clear( );
		m_EyeCascade.detectMultiScale( matFace, vecEyes, 1.1, 7, CV_HAAR_SCALE_IMAGE, Size( 30, 30 ) );
		for( j = 0; j < vecEyes.size( ); j++ )
			aFaces[ i ].vecEyes.push_back( CBBox( aFaces[ i ].boxFace, vecEyes[ j ], "BBox_Eye" ) );
	}

	//imgGray
	//->aFaces[ x ].boxFace
	//  ->aFaces[ x ].vecEyes[ y ]
	return aFaces;
}