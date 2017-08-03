#include "GazeData.h"
#include "Landmark.h"
#include "Image.h"
#include "Scenery.h"
#include "Render/RenderHelper.h"
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

std::vector<CGazeData> CGazeData::GetGazeData( std::vector<CGazeCapture> vecGaze, double dEyeDistance, double dFOV, CVector<3> vec3ScreenTL, CVector<3> vec3ScreenDim, const char *szWindow )
{
	const double dTanFOV = tan( dFOV * M_PI / ( 2 * 180 ) );
	std::vector<CGazeData> vecData;

	for( std::vector<CGazeCapture>::iterator pGaze = vecGaze.begin( ); pGaze < vecGaze.end( ); pGaze++ )
	{
		CVector<3> vec3Point( 
		{
			-( vec3ScreenTL[ 0 ] + pGaze->ptGaze.GetRelPositionX( -1 ) * vec3ScreenDim[ 0 ] ),
			vec3ScreenTL[ 1 ] + pGaze->ptGaze.GetRelPositionY( -1 ) * vec3ScreenDim[ 1 ],
			vec3ScreenTL[ 2 ]
		} );
		
		pGaze->imgGaze.Show( szWindow );
		cv::waitKey( 0 );

		std::vector<CLandmark> vecLandmarks = CLandmark::GetLandmarks( pGaze->imgGaze, szWindow );
		for( std::vector<CLandmark>::iterator it = vecLandmarks.begin( ); it < vecLandmarks.end( ); it++ )
		{
			double dWidth;
			double dHeight;
			{
				CImage *pImage = it->boxFace.GetImage( -1 );
				dWidth = pImage->GetWidth( );
				dHeight = pImage->GetHeight( );
			}
			double dPixelDif;
			{
				CVector<2> vec2EyeLeft( { (double) ( it->ptEyeLeft.GetPositionX( -1 ) ), (double) ( it->ptEyeLeft.GetPositionY( -1 ) ) } );
				CVector<2> vec2EyeRight( { (double) ( it->ptEyeRight.GetPositionX( -1 ) ), (double) ( it->ptEyeRight.GetPositionY( -1 ) ) } );
				dPixelDif = ( vec2EyeRight - vec2EyeLeft ).Abs( );
			}
			double dPixelDiagonal = sqrt( dWidth * dWidth + dHeight * dHeight );
			double dDistance = GetDistance( dEyeDistance, dPixelDif, dPixelDiagonal, dTanFOV );

			CVector<3> vec3EyeLeft(
			{
				GetPosition( dDistance, ( 0.5 - it->ptEyeLeft.GetRelPositionX( -1 ) ) * dWidth, dPixelDiagonal, dTanFOV ),
				GetPosition( dDistance, ( 0.5 - it->ptEyeLeft.GetRelPositionY( -1 ) ) * dHeight, dPixelDiagonal, dTanFOV ),
				dDistance
			} );

			CVector<3> vec3EyeRight(
			{
				GetPosition( dDistance, ( 0.5 - it->ptEyeRight.GetRelPositionX( -1 ) ) * dWidth, dPixelDiagonal, dTanFOV ),
				GetPosition( dDistance, ( 0.5 - it->ptEyeRight.GetRelPositionY( -1 ) ) * dHeight, dPixelDiagonal, dTanFOV ),
				dDistance
			} );

			vecData.emplace_back( vec3Point, vec3EyeLeft, vec3EyeRight );
		}
	}

	return vecData;
}

double CGazeData::GetDistance( double dMeterDif, double dPixelDif, double dPixelDiagonal, double dTanFOV )
{
	//Calculate distance from camera
	//tan( a ) = g1 / d <=> d = g1 / tan( a )
	//g1 / g2 = w / dif <=> g1 = w * g2 / dif
	//=> d = ( w * g2 ) / ( dif * tan( a ) ) = ( w / dif ) * ( g2 / tan( a ) )
	/*
	  +---
	  |   ---
	  |      ---
	  |         ---
	g1+-----       ---
	  |     ------    ---
	  g2          ------ ---
	  |                 ---a\
	  +-----------d----------+
	*/
	dMeterDif /= 2;
	return ( dPixelDiagonal / dPixelDif ) * ( dMeterDif / dTanFOV );
}

double CGazeData::GetPosition( double dDistance, double dPixelDif, double dPixelDiagonal, double dTanFOV )
{
	//tan( a ) = g1 / d <=> g1 = tan( a ) * d
	//g1 / g2 = w / dif <=> g2 = g1 * dif / w = tan( a ) * d * dif / w
	return dTanFOV * dDistance * dPixelDif / dPixelDiagonal;
}

bool CGazeData::DrawScenery( const char *szWindow )
{
	CScenery scenery( m_rayEyeLeft, m_rayEyeRight );
	CImage img( "Image_Scenery" );
	img.matImage = cv::Mat( 1050, 1050, CV_8UC3, cv::Scalar( 0, 0, 0 ) );

	//Write distance
	char szDistance[ 8 ];
	sprintf( szDistance, "%5.1fcm", m_rayEyeLeft.m_vec3Origin[ 2 ] * 100 );
	int iBaseline = 0;
	Size textSize = getTextSize( szDistance, FONT_HERSHEY_SIMPLEX, 1, 3, &iBaseline );
	iBaseline += 3;
	Point ptText( img.matImage.cols - textSize.width - 5, textSize.height + 5 );

	unsigned char cKey;
	bool fContinue = true;
	double dDegX = 211;
	double dDegY = 42;
	double dDegZ = 0;
	while( fContinue )
	{
		CImage imgDraw( img, "Image_Draw" );
		
		//printf( "Rot: (%f, %f, %f)\n", dDegX, dDegY, dDegZ );
		scenery.Transformed( CRenderHelper::GetRotationMatrix( dDegX, dDegY, dDegZ ) ).Fit( ).Draw( imgDraw );
		putText( imgDraw.matImage, szDistance, ptText,  FONT_HERSHEY_SIMPLEX, 1, Scalar( 255, 255, 255 ), 3 );
		imgDraw.Show( szWindow );

		cKey = (unsigned char) cv::waitKey( 0 );
		switch( cKey )
		{
		case 10:	//Enter
			fContinue = false;
			break;
		case 27:	//Escape
			return false;
		case 80:	//Pos1
			dDegX = 211;
			dDegY = 42;
			dDegZ = 0;
			break;
		case 81:	//Key)_Left
			dDegY -= 1;
			if( dDegY < 0 )
				dDegY = 360;

			break;
		case 82:	//Key_Up
			dDegX -= 1;
			if( dDegX < 0 )
				dDegX = 360;

			break;
		case 83:	//Key_Right
			dDegY += 1;
			if( dDegY >= 360 )
				dDegY = 0;

			break;
		case 84:	//Key_Down
			dDegX += 1;
			if( dDegX >= 360 )
				dDegX = 0;

			break;
		case 85:	//Img_Up
			dDegZ -= 1;
			if( dDegZ < 0 )
				dDegZ = 360;

			break;
		case 86:	//Img_Down
			dDegZ += 1;
			if( dDegZ >= 360 )
				dDegZ = 0;

			break;
		case 176:	//Numpad_0
			dDegX = 90;
			dDegY = 0;
			dDegZ = 0;
			break;
		case 178:	//Numpad_2
			dDegX = 180;
			dDegY = 0;
			dDegZ = 0;
			break;
		case 180:	//Numpad_4
			dDegX = 180;
			dDegY = 90;
			dDegZ = 0;
			break;
		case 181:	//Numpad_5
			dDegX = 270;
			dDegY = 0;
			dDegZ = 0;
			break;
		case 182:	//Numpad_6
			dDegX = 180;
			dDegY = 270;
			dDegZ = 0;
			break;
		case 184:	//Numpad_8
			dDegX = 0;
			dDegY = 0;
			dDegZ = 180;
			break;
		}
	}

	return true;
}