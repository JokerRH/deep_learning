#include "Landmark.h"
#include "Image.h"
#include <deque>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

/*
cv::threshold( input_frame, output_frame, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

------

dilate(input_frame,  output_frame, Mat(), Point(-1, -1), 5, 1, 1);
erode(input_frame,  output_frame, Mat(), Point(-1, -1), 5, 1, 1);
----

 vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( input_frame, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );


    int largest_area=0;

    Rect bounding_rect;
    double a;
    for( int i = 0; i< contours.size(); i++ ) // iterate through each contour.
       {
        a=contourArea( contours[i],false);  //  Find the area of contour
        if(a>largest_area){
        largest_area=a;

        bounding_rect=boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
        }

       }
------

rectangle( input_frame,bounding_rect.tl(), bounding_rect.br(), 255, 2, 8, 0 );
*/

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

CPoint CLandmark::GetPoint( CBBox &box )
{
	CImage imgFocus( "Image_Focus" );
	imgFocus.Crop( box );
	
	cvtColor( imgFocus.matImage, imgFocus.matImage, CV_BGR2GRAY );
	equalizeHist( imgFocus.matImage, imgFocus.matImage );

	threshold( imgFocus.matImage, imgFocus.matImage, 3, 255, CV_THRESH_BINARY );
	//dilate( imgFocus.matImage, imgFocus.matImage, Mat( ), Point( -1, -1 ), 5, 1, 1);
	//erode( imgFocus.matImage, imgFocus.matImage, Mat( ), Point( -1, -1 ), 5, 1, 1);
	imgFocus.Show( "Window" );
	waitKey( 0 );

	return CPoint( box, 0.5, 0.5, "Point_Eye" );
}

CPoint CLandmark::GetPointManual( CBBox &box, const char *szWindow )
{
	double dX = 0.5;
	double dY = 0.5;
	double dStepX = 1.0 / box.GetWidth( -1 );
	double dStepY = 1.0 / box.GetHeight( -1 );
	unsigned char cKey;
	CImage imgFocus;
	CImage imgDraw;
	CPoint pt;
	
	imgFocus.Crop( box );
	while( true )
	{
		imgDraw = CImage( imgFocus, "Image_Draw" );
		pt = CPoint( imgDraw, dX, dY, "Point_Eye" );
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

std::vector<CLandmark> CLandmark::GetLandmarks( std::vector<CLandmarkCandidate> vecCandidates, const char *szWindow )
{
	std::vector<CLandmark> vecLandmarks;
	for( std::vector<CLandmarkCandidate>::iterator pCandidate = vecCandidates.begin( ); pCandidate < vecCandidates.end( ); pCandidate++ )
	{
		try
		{
			vecLandmarks.emplace_back( *pCandidate, szWindow );
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

CLandmark::CLandmark( CLandmarkCandidate &candidate, const char *szWindow ) :
	boxFace( candidate.boxFace )
{
	if( candidate.aEyes.size( ) < 2 )
		throw( 0 );

	CBBox boxEyeLeft = GetEyeBox( candidate.aEyes, CVector<2>( { 0.67, 0.398 } ) );
	CBBox boxEyeRight = GetEyeBox( candidate.aEyes, CVector<2>( { 0.3, 0.398 } ) );

	boxEyeLeft.TransferOwnership( boxFace );
	boxEyeRight.TransferOwnership( boxFace );

	GetPoint( boxEyeLeft );
	GetPoint( boxEyeRight );

	while( true )
	{
		ptEyeLeft = GetPointManual( boxEyeLeft, szWindow );
		ptEyeRight = GetPointManual( boxEyeRight, szWindow );
		ptEyeLeft.TransferOwnership( 1 );
		ptEyeRight.TransferOwnership( 1 );
	
		CImage imgDraw;
		imgDraw.Crop( candidate.boxFace );
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

void CLandmark::Draw( CImage &img )
{
	ptEyeLeft.Draw( img, Scalar( 0, 255, 255 ) );
	ptEyeRight.Draw( img, Scalar( 0, 255, 255 ) );
}