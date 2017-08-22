#include "Landmark.h"
#include "Image.h"
#include <deque>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

/*
Eye Left: (0.664407, 0.396610), 0.203390x0.135593
Eye Right: (0.301695, 0.379661), 0.284746x0.189831
Eye Left: (0.673540, 0.400344), 0.199313x0.134021
Eye Right: (0.300687, 0.386598), 0.264605x0.175258
Eye Left: (0.677700, 0.398955), 0.205575x0.135889
Eye Right: (0.299652, 0.386760), 0.292683x0.195122
Eye Left: (0.668966, 0.387931), 0.213793x0.141379
Eye Right: (0.296552, 0.382759), 0.268966x0.179310
Eye Left: (0.684028, 0.392361), 0.208333x0.138889
Eye Right: (0.300347, 0.383681), 0.274306x0.184028
*/
CBBox CLandmark::GetEyeBox( const std::deque<CBBox> &vecEyes, const CVector<2> &vec2Pos )
{
	std::deque<CBBox>::const_iterator itClosest;
	double dClosest = 1;
	for( std::deque<CBBox>::const_iterator it = vecEyes.begin( ); it < vecEyes.end( ); it++ )
	{
		double d = ( it->GetCenter( ) - vec2Pos ).Abs2( );
		if( d < dClosest )
		{
			dClosest = d;
			itClosest = it;
		}
	}
	return *itClosest;
}

struct greater_than_key
{
	inline bool operator()( const vector<Point> &vec1, const vector<Point> &vec2 )
	{
		return ( contourArea( vec1, false ) > contourArea( vec2, false ) );
	}
};

CPoint CLandmark::GetPoint( CBBox &box )
{
	CImage imgFocus( "Image_Focus" );
	imgFocus.Crop( box );
	
	cvtColor( imgFocus.matImage, imgFocus.matImage, CV_BGR2GRAY );
	equalizeHist( imgFocus.matImage, imgFocus.matImage );

	threshold( imgFocus.matImage, imgFocus.matImage, 5, 255, CV_THRESH_BINARY );
	dilate( imgFocus.matImage, imgFocus.matImage, Mat( ), Point( -1, -1 ), 2 );
	erode( imgFocus.matImage, imgFocus.matImage, Mat( ), Point( -1, -1 ), 2 );
	
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours( imgFocus.matImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point( 0, 0 ) );

	std::sort( contours.begin( ), contours.end( ), greater_than_key() );
	
	CPoint pt;
	if( contours.size( ) >= 2 )
	{
		Point2f ptCenter;
		float rRadius;
		minEnclosingCircle( contours[ 1 ], ptCenter, rRadius );
		pt = CPoint( box, ptCenter, 0, "Point_Eye" );
	}
	else
	{
		pt = CPoint( box, 0.5, 0.5, "Point_Eye" );
	}

	return pt;
}

CPoint CLandmark::GetPointManual( CBBox &box, CPoint pt, const char *szWindow )
{
	pt.TransferOwnership( box );
	double dX = pt.GetRelPositionX( 0 );
	double dY = pt.GetRelPositionY( 0 );
	double dStepX = 1.0 / box.GetWidth( -1 );
	double dStepY = 1.0 / box.GetHeight( -1 );
	unsigned char cKey;
	CImage imgFocus;
	
	imgFocus.Crop( box );
	while( true )
	{
		CImage imgDraw( imgFocus, "Image_Draw" );
		CPoint pt = CPoint( imgDraw, dX, dY, "Point_Eye" );
		pt.Draw( Scalar( 0, 255, 255 ), 1, -1, 0 );
		imgDraw.Show( szWindow );

		cKey = (unsigned char) waitKey( 0 );
		switch( cKey )
		{
		case 8:		//Backspace
			throw( 0 );
		case 141:	//Numpad enter
		case 10:	//Enter
			//?
			//->box
			// ->imgFocus
			//  ->imgDraw
			//   ->pt
			pt.TransferOwnership( 3 );
			return pt;
		case 171:	//Numpad +
		case '+':	//+
			dStepX *= 0.5;
			dStepY *= 0.5;
			break;
		case 173:	//Numpad -
		case '-':	//-
			dStepX *= 2;
			dStepY *= 2;
			break;
		case '0':	//0
			dStepX = 0.05;
			dStepY = 0.05;
			break;
		case 81:	//Key)_Left
			dX -= dStepX;
			if( dX < 0 )
				dX = 0;

			break;
		case 82:	//Key_Up
			dY -= dStepY;
			if( dY < 0 )
				dY = 0;

			break;
		case 83:	//Key_Right
			dX += dStepX;
			if( dX > 1 )
				dX = 1;

			break;
		case 84:	//Key_Down
			dY += dStepY;
			if( dY > 1 )
				dY = 1;

			break;
		case 27:	//Escape
			throw( 1 );
		}
	}
}

void CLandmark::Adjust( const char *szWindow )
{
	CBBox boxEyeLeft( boxFace, ptEyeLeft.GetRelPositionX( 0 )- 0.1, ptEyeLeft.GetRelPositionY( 0 ) - 0.075, 0.2, 0.15, "Box_EyeLeft" );
	CBBox boxEyeRight( boxFace, ptEyeRight.GetRelPositionX( 0 )- 0.1, ptEyeRight.GetRelPositionY( 0 ) - 0.075, 0.2, 0.15, "Box_EyeRight" );

	while( true )
	{
		ptEyeLeft = GetPointManual( boxEyeLeft, ptEyeLeft, szWindow );
		ptEyeRight = GetPointManual( boxEyeRight, ptEyeRight, szWindow );
		ptEyeLeft.TransferOwnership( 1 );
		ptEyeRight.TransferOwnership( 1 );

		CImage imgDraw;
		imgDraw.Crop( boxFace );
		Draw( imgDraw );
		imgDraw.Show( szWindow );
		unsigned char cKey;
		bool fContinue = true;
		while( fContinue )
		{
			cKey = waitKey( 0 );
			switch( cKey )
			{
			case 8:		//Backspace
				throw( 0 );
			case 141:	//Numpad enter
			case 10:	//Enter
				return;
			case 27:	//Escape
				throw( 1 );
			case 'e':
				fContinue = false;
				break;
			}
		}
	}
}

std::vector<CLandmark> CLandmark::GetLandmarks( std::vector<CLandmarkCandidate> vecCandidates )
{
	std::vector<CLandmark> vecLandmarks;
	for( std::vector<CLandmarkCandidate>::iterator pCandidate = vecCandidates.begin( ); pCandidate < vecCandidates.end( ); pCandidate++ )
	{
		try
		{
			vecLandmarks.emplace_back( *pCandidate );
		}
		catch( int i )
		{
			switch( i )
			{
			case 0:	//Invalid candidate, skip
				break;
			case 1:	//User interrupt
			default:
				throw;
			}
		}
	}

	return vecLandmarks;
}

CLandmark::CLandmark( CLandmarkCandidate &candidate ) :
	boxFace( candidate.boxFace )
{
	if( candidate.aEyes.size( ) < 2 )
		throw( 0 );

	CBBox boxEyeLeft = GetEyeBox( candidate.aEyes, CVector<2>( { 0.67, 0.398 } ) );
	CBBox boxEyeRight = GetEyeBox( candidate.aEyes, CVector<2>( { 0.3, 0.398 } ) );

	boxEyeLeft.TransferOwnership( boxFace );
	boxEyeRight.TransferOwnership( boxFace );

	ptEyeLeft = GetPoint( boxEyeLeft );
	ptEyeRight = GetPoint( boxEyeRight );
	ptEyeLeft.TransferOwnership( 1 );
	ptEyeRight.TransferOwnership( 1 );
}

CLandmark::CLandmark( const CBBox &boxFace, const CPoint &ptEyeLeft, const CPoint &ptEyeRight ) :
	boxFace( boxFace ),
	ptEyeLeft( ptEyeLeft ),
	ptEyeRight( ptEyeRight )
{
	this->ptEyeLeft.TransferOwnership( this->boxFace );
	this->ptEyeRight.TransferOwnership( this->boxFace );
}

void CLandmark::Draw( CImage &img )
{
	ptEyeLeft.Draw( img, Scalar( 0, 255, 255 ) );
	ptEyeRight.Draw( img, Scalar( 0, 255, 255 ) );
}