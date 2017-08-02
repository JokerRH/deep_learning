#include "GazeData.h"
#include "Landmark.h"
#include "Image.h"
#include "Scenery.h"
#include "RenderHelper.h"
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

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
			printf( "Distance: %f\n", dDistance );
			
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
			printf( "Added data: EyeLeft: %s; EyeRight: %s\n", vecData.back( ).m_rayEyeLeft.ToString( ).c_str( ), vecData.back( ).m_rayEyeRight.ToString( ).c_str( ) );
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

	unsigned char cKey;
	bool fContinue = true;
	double dDegX = 0;
	double dDegY = 0;
	while( fContinue )
	{
		CImage imgDraw( img, "Image_Draw" );
		
		scenery.Transform( CRenderHelper::GetRotationMatrix( dDegX, dDegY, 0 ) ).Fit( ).Draw( imgDraw );
		imgDraw.Show( szWindow );

		cKey = (unsigned char) cv::waitKey( 0 );
		switch( cKey )
		{
		case 10:	//Enter
			fContinue = false;
			break;
		case 27:	//Escape
			return false;
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
			if( dDegY > 360 )
				dDegY = 0;

			break;
		case 84:	//Key_Down
			dDegX += 1;
			if( dDegX > 360 )
				dDegX = 0;

			break;
		}
	}

	return true;
}