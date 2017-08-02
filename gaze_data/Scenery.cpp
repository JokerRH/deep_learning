#include "Scenery.h"
#include "Line.h"
#include "ArrowedLine.h"
#include "Point.h"
#include "RenderHelper.h"
#include <algorithm>
#include <opencv2/core/types.hpp>

using namespace cv;

CVector<3> CScenery::s_vec3MonitorPos( { 0 } );
CVector<3> CScenery::s_vec3MonitorDim( { 0 } );

void CScenery::SetScenery( const CVector<3> &vec3MonitorPos, const CVector<3> vec3MonitorDim )
{
	s_vec3MonitorPos = vec3MonitorPos;
	s_vec3MonitorDim = vec3MonitorDim;
}

CScenery::CScenery( const CRay &rayEyeLeft, const CRay &rayEyeRight ) :
	CScenery( s_vec3MonitorPos, s_vec3MonitorDim, rayEyeLeft, rayEyeRight )
{

}

CScenery::CScenery( const CVector<3> &vec3MonitorPos, const CVector<3> &vec3MonitorDim, const CRay &rayEyeLeft, const CRay &rayEyeRight ) :
	m_Monitor(
	{
		vec3MonitorPos,																						//TLF
		vec3MonitorPos + CVector<3>( { 0, vec3MonitorDim[ 1 ], 0 } ),										//BLF
		vec3MonitorPos + CVector<3>( { vec3MonitorDim[ 0 ], vec3MonitorDim[ 1 ], 0 } ),						//BRF
		vec3MonitorPos + CVector<3>( { vec3MonitorDim[ 0 ], 0, 0 } ),										//TRF
		vec3MonitorPos + CVector<3>( { 0, 0, vec3MonitorDim[ 2 ] } ),										//TLB
		vec3MonitorPos + CVector<3>( { 0, vec3MonitorDim[ 1 ], vec3MonitorDim[ 2 ] } ),						//BLB
		vec3MonitorPos + CVector<3>( { vec3MonitorDim[ 0 ], vec3MonitorDim[ 1 ], vec3MonitorDim[ 2 ] } ),	//BRB
		vec3MonitorPos + CVector<3>( { vec3MonitorDim[ 0 ], 0, vec3MonitorDim[ 2 ] } ),						//TRB
	} ),
	m_Face(
	{
		CVector<3>( { -1.25, 5.0 / 6, 0 } ),	//TLF
		CVector<3>( { -1.25, -5.0 / 3, 0 } ),	//BLF
		CVector<3>( { 1.25, -5.0 / 3, 0 } ),	//BRF
		CVector<3>( { 1.25, 5.0 / 6, 0 } ),		//TRF
		CVector<3>( { -1.25, 5.0 / 6, -2 } ),	//TLB
		CVector<3>( { -1.25, -5.0 / 3, -2 } ),	//BLB
		CVector<3>( { 1.25, -5.0 / 3, -2 } ),	//BRB
		CVector<3>( { 1.25, 5.0 / 6, -2 } ),	//TRB
	} ),
	m_avec3Monitor
	{
		vec3MonitorPos,																						//TLF
		vec3MonitorPos + CVector<3>( { 0, vec3MonitorDim[ 1 ], 0 } ),										//BLF
		vec3MonitorPos + CVector<3>( { vec3MonitorDim[ 0 ], vec3MonitorDim[ 1 ], 0 } ),						//BRF
		vec3MonitorPos + CVector<3>( { vec3MonitorDim[ 0 ], 0, 0 } ),										//TRF
		vec3MonitorPos + CVector<3>( { 0, 0, vec3MonitorDim[ 2 ] } ),										//TLB
		vec3MonitorPos + CVector<3>( { 0, vec3MonitorDim[ 1 ], vec3MonitorDim[ 2 ] } ),						//BLB
		vec3MonitorPos + CVector<3>( { vec3MonitorDim[ 0 ], vec3MonitorDim[ 1 ], vec3MonitorDim[ 2 ] } ),	//BRB
		vec3MonitorPos + CVector<3>( { vec3MonitorDim[ 0 ], 0, vec3MonitorDim[ 2 ] } ),						//TRB
	},
	m_vec3Camera( { 0, 0, 0 } ),
	m_avec3Face
	{
		CVector<3>( { -1.25, 5.0 / 6, 0 } ),	//TLF
		CVector<3>( { -1.25, -5.0 / 3, 0 } ),	//BLF
		CVector<3>( { 1.25, -5.0 / 3, 0 } ),	//BRF
		CVector<3>( { 1.25, 5.0 / 6, 0 } ),		//TRF
		CVector<3>( { -1.25, 5.0 / 6, -2 } ),	//TLB
		CVector<3>( { -1.25, -5.0 / 3, -2 } ),	//BLB
		CVector<3>( { 1.25, -5.0 / 3, -2 } ),	//BRB
		CVector<3>( { 1.25, 5.0 / 6, -2 } ),	//TRB
	},
	m_rayEyeLeft( rayEyeLeft ),
	m_rayEyeRight( rayEyeRight )
{
	//Adjust Y-Values
	for( unsigned int u = 0; u < 8; u++ )
	{
		m_avec3Monitor[ u ][ 1 ] = -m_avec3Monitor[ u ][ 1 ];
		m_avec3Face[ u ][ 1 ] = -m_avec3Face[ u ][ 1 ];
	}
	
	m_rayEyeLeft.m_vec3Origin[ 1 ] = -m_rayEyeLeft.m_vec3Origin[ 1 ];
	m_rayEyeLeft.m_vec3Dir[ 1 ] = -m_rayEyeLeft.m_vec3Dir[ 1 ];
	m_rayEyeRight.m_vec3Origin[ 1 ] = -m_rayEyeRight.m_vec3Origin[ 1 ];
	m_rayEyeRight.m_vec3Dir[ 1 ] = -m_rayEyeRight.m_vec3Dir[ 1 ];
	
	//Scale and transform Face
	CVector<3> vec3EyesX = m_rayEyeRight.m_vec3Origin - m_rayEyeLeft.m_vec3Origin;
	CVector<3> vec3EyesZ( { 0, 0, -1 } );
	CVector<3> vec3EyesY = vec3EyesX.CrossProduct( vec3EyesZ );
	double dScale = vec3EyesX.Abs( );
	CMatrix<3, 3> matTransform = CRenderHelper::GetTransformationMatrix( vec3EyesX, vec3EyesY, vec3EyesZ );
	CVector<3> vec3Offset = ( m_rayEyeRight.m_vec3Origin + m_rayEyeLeft.m_vec3Origin ) / 2;
	for( unsigned int u = 0; u < 8; u++ )
	{
		m_avec3Face[ u ] *= dScale;
		m_avec3Face[ u ] = matTransform * m_avec3Face[ u ];
		m_avec3Face[ u ] += vec3Offset;
	}
}

CScenery CScenery::Transform( const CMatrix<3, 3> &matTransform ) const
{
	CScenery scenery;
	for( unsigned int u = 0; u < 8; u++ )
	{
		scenery.m_avec3Monitor[ u ] = matTransform * m_avec3Monitor[ u ];
		scenery.m_avec3Face[ u ] = matTransform * m_avec3Face[ u ];
	}

	scenery.m_vec3Camera = matTransform * m_vec3Camera;
	scenery.m_rayEyeLeft = m_rayEyeLeft.Transform( matTransform );
	scenery.m_rayEyeRight = m_rayEyeRight.Transform( matTransform );
	return scenery;
}

CScenery &CScenery::Transformed( const CMatrix<3, 3> &matTransform )
{
	for( unsigned int u = 0; u < 8; u++ )
	{
		m_avec3Monitor[ u ] = matTransform * m_avec3Monitor[ u ];
		m_avec3Face[ u ] = matTransform * m_avec3Face[ u ];
	}

	m_vec3Camera = matTransform * m_vec3Camera;
	m_rayEyeLeft.Transformed( matTransform );
	m_rayEyeRight.Transformed( matTransform );
	return *this;
}

CScenery &CScenery::Fit( void )
{
	CVector<3> vec3Min( { 0, 0, 0 } );
	for( unsigned int u = 0; u < 8; u++ )
	{
		vec3Min[ 0 ] = std::min( vec3Min[ 0 ], m_avec3Monitor[ u ][ 0 ] );
		vec3Min[ 1 ] = std::min( vec3Min[ 1 ], m_avec3Monitor[ u ][ 1 ] );
		vec3Min[ 2 ] = std::min( vec3Min[ 2 ], m_avec3Monitor[ u ][ 2 ] );
		
		vec3Min[ 0 ] = std::min( vec3Min[ 0 ], m_avec3Face[ u ][ 0 ] );
		vec3Min[ 1 ] = std::min( vec3Min[ 1 ], m_avec3Face[ u ][ 1 ] );
		vec3Min[ 2 ] = std::min( vec3Min[ 2 ], m_avec3Face[ u ][ 2 ] );
	}

	vec3Min[ 0 ] = std::min( vec3Min[ 0 ], m_vec3Camera[ 0 ] );
	vec3Min[ 1 ] = std::min( vec3Min[ 1 ], m_vec3Camera[ 1 ] );
	vec3Min[ 2 ] = std::min( vec3Min[ 2 ], m_vec3Camera[ 2 ] );
	
	vec3Min[ 0 ] = std::min( vec3Min[ 0 ], m_rayEyeLeft.m_vec3Origin[ 0 ] );
	vec3Min[ 1 ] = std::min( vec3Min[ 1 ], m_rayEyeLeft.m_vec3Origin[ 1 ] );
	vec3Min[ 2 ] = std::min( vec3Min[ 2 ], m_rayEyeLeft.m_vec3Origin[ 2 ] );
	
	vec3Min[ 0 ] = std::min( vec3Min[ 0 ], m_rayEyeRight.m_vec3Origin[ 0 ] );
	vec3Min[ 1 ] = std::min( vec3Min[ 1 ], m_rayEyeRight.m_vec3Origin[ 1 ] );
	vec3Min[ 2 ] = std::min( vec3Min[ 2 ], m_rayEyeRight.m_vec3Origin[ 2 ] );

	CVector<3> vec3Max( { 0, 0, 0 } );
	for( unsigned int u = 0; u < 8; u++ )
	{
		m_avec3Monitor[ u ] -= vec3Min;
		vec3Max[ 0 ] = std::max( vec3Max[ 0 ], m_avec3Monitor[ u ][ 0 ] );
		vec3Max[ 1 ] = std::max( vec3Max[ 1 ], m_avec3Monitor[ u ][ 1 ] );
		vec3Max[ 2 ] = std::max( vec3Max[ 2 ], m_avec3Monitor[ u ][ 2 ] );
		
		m_avec3Face[ u ] -= vec3Min;
		vec3Max[ 0 ] = std::max( vec3Max[ 0 ], m_avec3Face[ u ][ 0 ] );
		vec3Max[ 1 ] = std::max( vec3Max[ 1 ], m_avec3Face[ u ][ 1 ] );
		vec3Max[ 2 ] = std::max( vec3Max[ 2 ], m_avec3Face[ u ][ 2 ] );
	}

	m_vec3Camera -= vec3Min;
	vec3Max[ 0 ] = std::max( vec3Max[ 0 ], m_vec3Camera[ 0 ] );
	vec3Max[ 1 ] = std::max( vec3Max[ 1 ], m_vec3Camera[ 1 ] );
	vec3Max[ 2 ] = std::max( vec3Max[ 2 ], m_vec3Camera[ 2 ] );
	
	m_rayEyeLeft.m_vec3Origin -= vec3Min;
	vec3Max[ 0 ] = std::max( vec3Max[ 0 ], m_rayEyeLeft.m_vec3Origin[ 0 ] );
	vec3Max[ 1 ] = std::max( vec3Max[ 1 ], m_rayEyeLeft.m_vec3Origin[ 1 ] );
	vec3Max[ 2 ] = std::max( vec3Max[ 2 ], m_rayEyeLeft.m_vec3Origin[ 2 ] );
	
	vec3Max[ 0 ] = std::max( vec3Max[ 0 ], m_rayEyeLeft( 1 )[ 0 ] );
	vec3Max[ 1 ] = std::max( vec3Max[ 1 ], m_rayEyeLeft( 1 )[ 1 ] );
	vec3Max[ 2 ] = std::max( vec3Max[ 2 ], m_rayEyeLeft( 1 )[ 2 ] );
	
	m_rayEyeRight.m_vec3Origin -= vec3Min;
	vec3Max[ 0 ] = std::max( vec3Max[ 0 ], m_rayEyeRight.m_vec3Origin[ 0 ] );
	vec3Max[ 1 ] = std::max( vec3Max[ 1 ], m_rayEyeRight.m_vec3Origin[ 1 ] );
	vec3Max[ 2 ] = std::max( vec3Max[ 2 ], m_rayEyeRight.m_vec3Origin[ 2 ] );
	
	vec3Max[ 0 ] = std::max( vec3Max[ 0 ], m_rayEyeRight( 1 )[ 0 ] );
	vec3Max[ 1 ] = std::max( vec3Max[ 1 ], m_rayEyeRight( 1 )[ 1 ] );
	vec3Max[ 2 ] = std::max( vec3Max[ 2 ], m_rayEyeRight( 1 )[ 2 ] );

	double dScale = 1.0 / std::max( vec3Max[ 0 ], vec3Max[ 1 ] );
	CMatrix<3, 3> matScale(
	{
		dScale, 0, 0,
		0, dScale, 0,
		0, 0, dScale
	} );

	return Transformed( matScale );
}

CScenery::CScenery( void ) :
	m_Monitor( { CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ) } ),
	m_Face( { CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ) } ),
	m_avec3Monitor{ CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ) },
	m_vec3Camera( CVector<3>( { 0, 0, 0 } ) ),
	m_avec3Face{ CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ) },
	m_rayEyeLeft( CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ) ),
	m_rayEyeRight( CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ) )
{

}

#define LINEB( startp, endp, name )	CLine( img, startp[ 0 ], startp[ 1 ], endp[ 0 ], endp[ 1 ], "Line_" name ).Draw( Scalar( 255, 0, 0 ), 2, 8, 0, 0 );
#define LINER( startp, endp, name )	CLine( img, startp[ 0 ], startp[ 1 ], endp[ 0 ], endp[ 1 ], "Line_" name ).Draw( Scalar( 0, 0, 255 ), 2, 8, 0, 0 );
void CScenery::Draw( CImage &img ) const
{
	//Draw face
	cv::Point aFace[ 4 ] =
	{
		cv::Point( m_avec3Face[ 0 ][ 0 ] * img.matImage.cols, m_avec3Face[ 0 ][ 1 ] * img.matImage.rows ),
		cv::Point( m_avec3Face[ 1 ][ 0 ] * img.matImage.cols, m_avec3Face[ 1 ][ 1 ] * img.matImage.rows ),
		cv::Point( m_avec3Face[ 2 ][ 0 ] * img.matImage.cols, m_avec3Face[ 2 ][ 1 ] * img.matImage.rows ),
		cv::Point( m_avec3Face[ 3 ][ 0 ] * img.matImage.cols, m_avec3Face[ 3 ][ 1 ] * img.matImage.rows ),
	};
	fillConvexPoly( img.matImage, aFace, 4, Scalar( 0, 0, 127 ) );
	
	//Draw monitor
	cv::Point aMonitor[ 4 ] =
	{
		cv::Point( m_avec3Monitor[ 0 ][ 0 ] * img.matImage.cols, m_avec3Monitor[ 0 ][ 1 ] * img.matImage.rows ),
		cv::Point( m_avec3Monitor[ 1 ][ 0 ] * img.matImage.cols, m_avec3Monitor[ 1 ][ 1 ] * img.matImage.rows ),
		cv::Point( m_avec3Monitor[ 2 ][ 0 ] * img.matImage.cols, m_avec3Monitor[ 2 ][ 1 ] * img.matImage.rows ),
		cv::Point( m_avec3Monitor[ 3 ][ 0 ] * img.matImage.cols, m_avec3Monitor[ 3 ][ 1 ] * img.matImage.rows ),
	};
	fillConvexPoly( img.matImage, aMonitor, 4, Scalar( 127, 0, 0 ) );

	m_Monitor.RenderFrame( img, Scalar( 255, 0, 0 ) );
	/*
	//Draw monitor frame
	LINEB( m_avec3Monitor[ 0 ], m_avec3Monitor[ 1 ], "Mon_LF" );
	LINEB( m_avec3Monitor[ 1 ], m_avec3Monitor[ 2 ], "Mon_BF" );
	LINEB( m_avec3Monitor[ 2 ], m_avec3Monitor[ 3 ], "Mon_RF" );
	LINEB( m_avec3Monitor[ 3 ], m_avec3Monitor[ 0 ], "Mon_TF" );

	LINEB( m_avec3Monitor[ 0 ], m_avec3Monitor[ 4 ], "Mon_LC" );
	LINEB( m_avec3Monitor[ 1 ], m_avec3Monitor[ 5 ], "Mon_BC" );
	LINEB( m_avec3Monitor[ 2 ], m_avec3Monitor[ 6 ], "Mon_RC" );
	LINEB( m_avec3Monitor[ 3 ], m_avec3Monitor[ 7 ], "Mon_TC" );
	
	LINEB( m_avec3Monitor[ 4 ], m_avec3Monitor[ 5 ], "Mon_LB" );
	LINEB( m_avec3Monitor[ 5 ], m_avec3Monitor[ 6 ], "Mon_BB" );
	LINEB( m_avec3Monitor[ 6 ], m_avec3Monitor[ 7 ], "Mon_RB" );
	LINEB( m_avec3Monitor[ 7 ], m_avec3Monitor[ 4 ], "Mon_TB" );
	*/

	//Draw face frame
	LINER( m_avec3Face[ 0 ], m_avec3Face[ 1 ], "Face_LF" );
	LINER( m_avec3Face[ 1 ], m_avec3Face[ 2 ], "Face_BF" );
	LINER( m_avec3Face[ 2 ], m_avec3Face[ 3 ], "Face_RF" );
	LINER( m_avec3Face[ 3 ], m_avec3Face[ 0 ], "Face_TF" );

	LINER( m_avec3Face[ 0 ], m_avec3Face[ 4 ], "Face_LC" );
	LINER( m_avec3Face[ 1 ], m_avec3Face[ 5 ], "Face_BC" );
	LINER( m_avec3Face[ 2 ], m_avec3Face[ 6 ], "Face_RC" );
	LINER( m_avec3Face[ 3 ], m_avec3Face[ 7 ], "Face_TC" );

	LINER( m_avec3Face[ 4 ], m_avec3Face[ 5 ], "Face_LB" );
	LINER( m_avec3Face[ 5 ], m_avec3Face[ 6 ], "Face_BB" );
	LINER( m_avec3Face[ 6 ], m_avec3Face[ 7 ], "Face_RB" );
	LINER( m_avec3Face[ 7 ], m_avec3Face[ 4 ], "Face_TB" );

	//Draw gaze
	CLine( img, m_rayEyeLeft.m_vec3Origin[ 0 ], m_rayEyeLeft.m_vec3Origin[ 1 ], m_rayEyeLeft( 1 )[ 0 ], m_rayEyeLeft( 1 )[ 1 ], "Line_EyeLeft" ).Draw( Scalar( 0, 255, 0 ), 1, 8, 0, 0 );
	CLine( img, m_rayEyeRight.m_vec3Origin[ 0 ], m_rayEyeRight.m_vec3Origin[ 1 ], m_rayEyeRight( 1 )[ 0 ], m_rayEyeRight( 1 )[ 1 ], "Line_EyeRight" ).Draw( Scalar( 0, 255, 0 ), 1, 8, 0, 0 );
	
	//Get gaze point
	CVector<2> vec2Gaze = m_rayEyeLeft.PointOfShortestDistance( m_rayEyeRight );
	CVector<3> vec3Gaze = ( m_rayEyeLeft( vec2Gaze[ 0 ] ) + m_rayEyeRight( vec2Gaze[ 1 ] ) ) / 2;
	if( vec3Gaze[ 0 ] >= 0 && vec3Gaze[ 0 ] <= 1 && vec3Gaze[ 1 ] >= 0 && vec3Gaze[ 1 ] <= 1 )
		CPoint( img, vec3Gaze[ 0 ], vec3Gaze[ 1 ], "Point_Gaze" ).Draw( Scalar( 0, 0, 255 ), 3, -1, 0 );

	//Draw eyes
	CPoint( img, m_rayEyeLeft.m_vec3Origin[ 0 ], m_rayEyeLeft.m_vec3Origin[ 1 ], "Point_EyeLeft" ).Draw( Scalar( 0, 255, 255 ), 3, -1, 0 );
	CPoint( img, m_rayEyeRight.m_vec3Origin[ 0 ], m_rayEyeRight.m_vec3Origin[ 1 ], "Point_EyeRight" ).Draw( Scalar( 0, 255, 255 ), 3, -1, 0 );

	//Draw camera
	CPoint( img, m_vec3Camera[ 0 ], m_vec3Camera[ 1 ], "Point_Camera" ).Draw( Scalar( 0, 255, 255 ), 3, -1, 0 );
}
#undef LINE

std::string CScenery::ToString( unsigned int uPrecision ) const
{
	std::string s = "";
	s += "Monitor: " + m_avec3Monitor[ 0 ].ToString( uPrecision );
	for( unsigned int u = 1; u < 8; u++ )
		s += ", " + m_avec3Monitor[ u ].ToString( uPrecision );
		
	s += "; Camera: " + m_vec3Camera.ToString( uPrecision );
	s += "; Eyes: " + m_rayEyeLeft.ToString( uPrecision ) + ", " + m_rayEyeRight.ToString( uPrecision );

	s += "; Face: " + m_avec3Face[ 0 ].ToString( uPrecision );
	for( unsigned int u = 1; u < 8; u++ )
		s += ", " + m_avec3Face[ u ].ToString( uPrecision );
		
	return s;
}