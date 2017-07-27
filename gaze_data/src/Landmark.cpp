#include "Landmark.h"
#include "Image.h"
#include <deque>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

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
	Mat matFocus;
	Mat matDraw;
	CImage imgFocus( "Image_Focus", matFocus );
	CImage imgDraw( "Image_Draw", matDraw );

	//Crop face and draw; Switch eyes
	imgFocus.Crop( candidate.boxFace );
	unsigned char cKey;
	bool fContinue = true;
	while( fContinue )
	{
		imgDraw = CImage( imgFocus, "Image_Draw", matDraw );
		std::deque<CBBox>::iterator pboxEye = candidate.aEyes.begin( );
		pboxEye->Draw( imgDraw, Scalar( 0, 255, 0 ) );			//Draw left eye candidate green
		for( pboxEye++; pboxEye < candidate.aEyes.end( ); pboxEye++ )
			pboxEye->Draw( imgDraw, Scalar( 255, 0, 0 ) );		//Draw other eye candidates blue
		imgDraw.Show( szWindow );

		cKey = (unsigned char) waitKey( 0 );
		switch( cKey )
		{
		case 8:		//Backspace
			throw( 0 );
		case 9:		//Tab
			candidate.aEyes.push_back( candidate.aEyes.front( ) );
			candidate.aEyes.pop_front( );
			break;
		case 10:	//Enter
			fContinue = false;
			break;
		case 27:	//Escape
			throw( 1 );
		}
	}

	//Left eye selected, focus on eyes
	unsigned int uX;
	unsigned int uY;
	for( std::deque<CBBox>::iterator pboxEye = candidate.aEyes.begin( ); pboxEye < candidate.aEyes.end( ); )
	{
		//Crop eye and draw
		imgFocus.Crop( *pboxEye );
		imgDraw = CImage( imgFocus, "Image_Draw", matDraw );
		CPoint ptEye( imgDraw, 0.5, 0.5 );
		uX = ptEye.GetPositionX( 0 );
		uY = ptEye.GetPositionY( 0 );
		fContinue = true;
		while( fContinue )
		{
			matDraw = matFocus.clone( );
			ptEye = CPoint( imgDraw, cv::Point( uX, uY ) );
			ptEye.Draw( Scalar( 0, 255, 255 ), 1, -1, 0 );
			imgDraw.Show( szWindow );

			cKey = (unsigned char) waitKey( 0 );
			switch( cKey )
			{
			case 8:		//Backspace
				candidate.aEyes.erase( pboxEye );
				fContinue = false;
				break;
			case 10:	//Enter
				//?
				//->boxFace
				//->candidate.boxFace
				// ->candidate.aEyes[ x ]
				//  ->imgFocus (Eye)
				//   ->imgDraw (Eye)
				//    ->ptEye
				if( pboxEye == candidate.aEyes.begin( ) )
				{
					//First in list, left eye
					ptEyeLeft = ptEye;
					ptEyeLeft.TransferOwnership( boxFace );
					pboxEye++;
				}
				else
				{
					//Right eye
					ptEyeRight = ptEye;
					ptEyeRight.TransferOwnership( boxFace );
					pboxEye = candidate.aEyes.end( );
				}
				fContinue = false;
				break;
			case 81:	//Key_Left
				if( uX )
					uX--;
				break;
			case 82:	//Key_Up
				if( uY )
					uY--;
				break;
			case 83:	//Key_Right
				if( uX + 1 < ptEye.GetParent( 1 )->GetWidth( 0 ) )
					uX++;
				break;
			case 84:	//Key_Down
				if( uY + 1 < ptEye.GetParent( 1 )->GetHeight( 0 ) )
					uY++;
				break;
			case 27:	//Escape
				throw( 1 );
			}
		}
	}

	//Crop face and draw; Switch nose
	imgFocus.Crop( candidate.boxFace );
	fContinue = ( candidate.aNose.size( ) > 1 );
	while( fContinue )
	{
		imgDraw = CImage( imgFocus, "Image_Draw", matDraw );
		std::deque<CBBox>::iterator pboxNose = candidate.aNose.begin( );
		pboxNose->Draw( imgDraw, Scalar( 0, 255, 0 ) );			//Draw first nose candidate green
		for( pboxNose++; pboxNose < candidate.aNose.end( ); pboxNose++ )
			pboxNose->Draw( imgDraw, Scalar( 255, 0, 0 ) );		//Draw other nose candidates blue
		imgDraw.Show( szWindow );

		cKey = (unsigned char) waitKey( 0 );
		switch( cKey )
		{
		case 8:		//Backspace
			throw( 0 );
		case 9:		//Tab
			candidate.aNose.push_back( candidate.aNose.front( ) );
			candidate.aNose.pop_front( );
			break;
		case 10:	//Enter
			fContinue = false;
			break;
		case 27:	//Escape
			throw( 1 );
		}
	}
	
	//Nose selected
	imgFocus.Crop( candidate.aNose.front( ) );
	imgDraw = CImage( imgFocus, "Image_Draw", matDraw );
	CPoint ptNose( imgDraw, 0.5, 0.5 );
	uX = ptNose.GetPositionX( 0 );
	uY = ptNose.GetPositionY( 0 );
	fContinue = true;
	while( fContinue )
	{
		matDraw = matFocus.clone( );
		ptNose = CPoint( imgDraw, cv::Point( uX, uY ) );
		ptNose.Draw( Scalar( 0, 255, 255 ), 1, -1, 0 );
		imgDraw.Show( szWindow );
		cKey = (unsigned char) waitKey( 0 );
		switch( cKey )
		{
		case 8:		//Backspace
			throw( 0 );
		case 10:	//Enter
			//?
			//->boxFace
			//->candidate.boxFace
			// ->candidate.aEyes[ x ]
			//  ->imgFocus (Eye)
			//   ->imgDraw (Eye)
			//    ->ptEye
			this->ptNose = ptNose;
			this->ptNose.TransferOwnership( boxFace );
			fContinue = false;
			break;
		case 81:	//Key_Left
			if( uX )
				uX--;
			break;
		case 82:	//Key_Up
			if( uY )
				uY--;
			break;
		case 83:	//Key_Right
			if( uX + 1 < ptNose.GetParent( 1 )->GetWidth( 0 ) )
				uX++;
			break;
		case 84:	//Key_Down
			if( uY + 1 < ptNose.GetParent( 1 )->GetHeight( 0 ) )
				uY++;
			break;
		case 27:	//Escape
			throw( 1 );
		}
	}
	
	//Calculate distance from camera
	//tan( a ) = g1 / d <=> d = g1 / tan( a )
	//g1 / g2 = w / dif <=> g1 = w * g2 / dif
	//=> d = ( w * g2 ) / ( dif * tan( a ) ) = ( w / dif ) * ( g2 / tan( a ) )
	rDistance = ( (float) boxFace.GetImage( -1 )->GetWidth( ) / ( ptEyeLeft.GetPositionX( -1 ) - ptEyeRight.GetPositionX( -1 ) ) ) * ( 0.0325 / 0.7535540501 );
	printf( "Distance: %f\n", rDistance );

	//Draw face with features marked
	imgDraw.Crop( candidate.boxFace );
	Draw( imgDraw );
	imgDraw.Show( szWindow );

	fContinue = true;
	while( fContinue )
	{
		cKey = (unsigned char) waitKey( 0 );
		switch( cKey )
		{
		case 8:		//Backspace
			throw( 0 );
		case 10:	//Enter
			fContinue = false;
			break;
		case 27:	//Escape
			throw( 1 );
		}
	}
}

void CLandmark::Draw( CImage &img )
{
	ptEyeLeft.Draw( img, Scalar( 0, 255, 255 ) );
	ptEyeRight.Draw( img, Scalar( 0, 255, 255 ) );
	ptNose.Draw( img, Scalar( 0, 255, 255 ) );
	
	unsigned int uLX = ptEyeLeft.GetPositionX( );
	unsigned int uRX = ptEyeRight.GetPositionX( );
	unsigned int uLY = ptEyeLeft.GetPositionY( );
	unsigned int uRY = ptEyeRight.GetPositionY( );
	printf( "Eye left: (%u, %u); Eye right: (%u, %u); Distance: (%d, %d)\n", uLX, uLY, uRX, uRY, uLX - uRX, uLY - uRY );
}