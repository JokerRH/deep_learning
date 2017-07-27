#include "LandmarkCandidate.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

using namespace cv;

CascadeClassifier CLandmarkCandidate::s_FaceCascade;
CascadeClassifier CLandmarkCandidate::s_EyeCascade;
CascadeClassifier CLandmarkCandidate::s_NoseCascade;

std::vector<CLandmarkCandidate> CLandmarkCandidate::GetCandidates( CImage &img )
{
	Mat matGray;
	CImage imgGray( img, "Image_Gray", matGray );
	cvtColor( matGray, matGray, CV_BGR2GRAY );
	equalizeHist( matGray, matGray );

	std::vector<Rect> vecFaces;
	s_FaceCascade.detectMultiScale( *imgGray.pmatImage, vecFaces, 1.1, 5, CV_HAAR_SCALE_IMAGE, Size( 30, 30 ) );

	Mat matFace;
	CImage imgFace( "Image_Face", matFace );
	std::vector<CLandmarkCandidate> vecLandmarks;
	std::vector<Rect> vecDetected;
	printf( "Size: %lu\n", vecFaces.size( ) );
	for( std::vector<Rect>::iterator prectFace = vecFaces.begin( ); prectFace < vecFaces.end( ); prectFace++ )
	{
		vecLandmarks.emplace_back( CBBox( imgGray, *prectFace, "BBox_Face" ) );
		imgFace.Crop( vecLandmarks.back( ).boxFace );

		s_EyeCascade.detectMultiScale( matFace, vecDetected, 1.1, 10, CV_HAAR_SCALE_IMAGE, Size( 30, 30 ) );
		for( std::vector<Rect>::iterator prectEye = vecDetected.begin( ); prectEye < vecDetected.end( ); prectEye++ )
			vecLandmarks.back( ).aEyes.emplace_back( vecLandmarks.back( ).boxFace, *prectEye, "BBox_Eye" );

		s_NoseCascade.detectMultiScale( matFace, vecDetected, 1.1, 10, CV_HAAR_SCALE_IMAGE, Size( 30, 30 ) );
		for( std::vector<Rect>::iterator prectNose = vecDetected.begin( ); prectNose < vecDetected.end( ); prectNose++ )
			vecLandmarks.back( ).aNose.emplace_back( vecLandmarks.back( ).boxFace, *prectNose, "BBox_Nose" );

		vecLandmarks.back( ).boxFace.TransferOwnership( 1 );
		
		printf( "1:Face at %p; parent: \"%s\"\n", &vecLandmarks.back( ).boxFace, vecLandmarks.back( ).boxFace.GetParent( 1 )->GetName( ) );
		for( std::deque<CBBox>::iterator p = vecLandmarks.back( ).aEyes.begin( ); p < vecLandmarks.back( ).aEyes.end( ); p++ )
		{
			printf( "  Eye parent at %p: ", p->GetParent( 1 ) );
			printf( "\"%s\"\n", p->GetParent( 1 )->GetName( ) );
		}
		for( std::deque<CBBox>::iterator p = vecLandmarks.back( ).aNose.begin( ); p < vecLandmarks.back( ).aNose.end( ); p++ )
		{
			printf( "  Nose parent at %p: ", p->GetParent( 1 ) );
			printf( "\"%s\"\n", p->GetParent( 1 )->GetName( ) );
		}
	}
	
	for( std::vector<CLandmarkCandidate>::iterator it = vecLandmarks.begin( ); it < vecLandmarks.end( ); it++ )
	{
		printf( "Face at %p; parent: \"%s\"\n", &it->boxFace, it->boxFace.GetParent( 1 )->GetName( ) );
		for( std::deque<CBBox>::iterator p = it->aEyes.begin( ); p < it->aEyes.end( ); p++ )
		{
			printf( "  Eye parent at %p: ", p->GetParent( 1 ) );
			printf( "\"%s\"\n", p->GetParent( 1 )->GetName( ) );
		}
		for( std::deque<CBBox>::iterator p = it->aNose.begin( ); p < it->aNose.end( ); p++ )
		{
			printf( "  Nose parent at %p: ", p->GetParent( 1 ) );
			printf( "\"%s\"\n", p->GetParent( 1 )->GetName( ) );
		}
	}

	//img
	//->vecLandmarks[ x ].boxFace
	// ->vecLandmarks[ x ].aEyes[ y ]
	// ->vecLandmarks[ x ].aNose[ y ]
	return vecLandmarks;
}

void CLandmarkCandidate::Init( void )
{
	if( !s_FaceCascade.load( "./haarcascades/haarcascade_frontalface_default.xml" ) )
	{
		fprintf( stderr, "Unable to load face cascade\n" );
		throw 0;
	}

	if( !s_EyeCascade.load( "./haarcascades/haarcascade_mcs_lefteye.xml" ) )
	{
		fprintf( stderr, "Unable to load eye cascade\n" );
		throw 0;
	}
	
	if( !s_NoseCascade.load( "./haarcascades/haarcascade_mcs_nose.xml" ) )
	{
		fprintf( stderr, "Unable to load nose cascade\n" );
		throw 0;
	}
}

void CLandmarkCandidate::Draw( CImage &img )
{
	boxFace.Draw( img, Scalar( 0, 0, 255 ) );
	for( std::deque<CBBox>::iterator pboxEye = aEyes.begin( ); pboxEye < aEyes.end( ); pboxEye++ )
		pboxEye->Draw( img, Scalar( 255, 0, 0 ) );
	for( std::deque<CBBox>::iterator pboxNose = aNose.begin( ); pboxNose < aNose.end( ); pboxNose++ )
		pboxNose->Draw( img, Scalar( 0, 255, 0 ) );
}