#include "GazeData.h"
#include "Landmark.h"
#include "Image.h"
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

std::vector<CGazeData> CGazeData::GetGazeData( std::vector<CGazeCapture> vecGaze, double dEyeDistance, double dFOV, CVector<3> vec3ScreenTL, CVector<2> vec2ScreenDim, const char *szWindow )
{
	const double dTanFOV = tan( dFOV / 2 );
	std::vector<CGazeData> vecData;

	for( std::vector<CGazeCapture>::iterator pGaze = vecGaze.begin( ); pGaze < vecGaze.end( ); pGaze++ )
	{
		CVector<3> vec3Point( 
		{
			vec3ScreenTL[ 0 ] + pGaze->ptGaze.GetRelPositionX( -1 ) * vec2ScreenDim[ 0 ],
			vec3ScreenTL[ 1 ] + pGaze->ptGaze.GetRelPositionY( -1 ) * vec2ScreenDim[ 1 ],
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
			double dPixelDif = it->ptEyeLeft.GetPositionX( -1 ) - it->ptEyeRight.GetPositionX( -1 );
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