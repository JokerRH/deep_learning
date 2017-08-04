#include "LandmarkCandidate.h"
#include "Config.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

using namespace cv;

CascadeClassifier CLandmarkCandidate::s_FaceCascade;
CascadeClassifier CLandmarkCandidate::s_EyeCascade;

std::vector<CLandmarkCandidate> CLandmarkCandidate::GetCandidates( CImage &img )
{
	CImage imgGray( img, "Image_Gray" );
	cvtColor( imgGray.matImage, imgGray.matImage, CV_BGR2GRAY );
	equalizeHist( imgGray.matImage, imgGray.matImage );

	std::vector<Rect> vecFaces;
	s_FaceCascade.detectMultiScale( imgGray.matImage, vecFaces, 1.1, 5, CV_HAAR_SCALE_IMAGE, Size( 30, 30 ) );
	
	CImage imgFace( "Image_Face" );
	std::vector<CLandmarkCandidate> vecLandmarks;
	std::vector<Rect> vecDetected;
	for( std::vector<Rect>::iterator prectFace = vecFaces.begin( ); prectFace < vecFaces.end( ); prectFace++ )
	{
		vecLandmarks.emplace_back( CBBox( imgGray, *prectFace, -1, "BBox_Face" ) );
		imgFace.Crop( vecLandmarks.back( ).boxFace );

		s_EyeCascade.detectMultiScale( imgFace.matImage, vecDetected, 1.1, 10, CV_HAAR_SCALE_IMAGE, Size( 30, 30 ) );
		for( std::vector<Rect>::iterator prectEye = vecDetected.begin( ); prectEye < vecDetected.end( ); prectEye++ )
			vecLandmarks.back( ).aEyes.emplace_back( vecLandmarks.back( ).boxFace, *prectEye, -1, "BBox_Eye" );

		vecLandmarks.back( ).boxFace.TransferOwnership( 1 );
	}

	//img
	//->vecLandmarks[ x ].boxFace
	// ->vecLandmarks[ x ].aEyes[ y ]
	return vecLandmarks;
}

void CLandmarkCandidate::Init( void )
{
	if( !s_FaceCascade.load( HAARCASCADE_DIRECTORY "haarcascade_frontalface_default.xml" ) )
	{
		fprintf( stderr, "Unable to load face cascade\n" );
		throw 0;
	}

	if( !s_EyeCascade.load( HAARCASCADE_DIRECTORY "haarcascade_mcs_lefteye.xml" ) )
	{
		fprintf( stderr, "Unable to load eye cascade\n" );
		throw 0;
	}
}

void CLandmarkCandidate::Draw( CImage &img )
{
	boxFace.Draw( img, Scalar( 0, 0, 255 ) );
	for( std::deque<CBBox>::iterator pboxEye = aEyes.begin( ); pboxEye < aEyes.end( ); pboxEye++ )
		pboxEye->Draw( img, Scalar( 255, 0, 0 ) );
}