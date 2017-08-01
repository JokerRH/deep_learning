#include "Scenery.h"
#include "Line.h"
#include "ArrowedLine.h"
#include "Point.h"
#include <algorithm>
#include <opencv2/core/types.hpp>

using namespace cv;

CScenery::CScenery( const CVector<3> &vec3MonitorPos, const CVector<3> &vec3MonitorDim, const CRay &rayEyeLeft, const CRay &rayEyeRight ) :
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
		CVector<3>( { 0, 0, 0 } ),	//TLF
		CVector<3>( { 0, -1, 0 } ),	//BLF
		CVector<3>( { 1, -1, 0 } ),	//BRF
		CVector<3>( { 1, 0, 0 } ),	//TRF
		CVector<3>( { 0, 0, 1 } ),	//TLB
		CVector<3>( { 0, -1, 1 } ),	//BLB
		CVector<3>( { 1, -1, 1 } ),	//BRB
		CVector<3>( { 1, 0, 1 } )	//TRB
	},
	m_rayEyeLeft( rayEyeLeft ),
	m_rayEyeRight( rayEyeRight )
{
	for( unsigned int u = 0; u < 8; u++ )
	{
		m_avec3Monitor[ u ][ 1 ] = -m_avec3Monitor[ u ][ 1 ];
		m_avec3Face[ u ][ 1 ] = -m_avec3Face[ u ][ 1 ];
	}
	
	m_rayEyeLeft.m_vec3Origin[ 1 ] = -m_rayEyeLeft.m_vec3Origin[ 1 ];
	m_rayEyeLeft.m_vec3Dir[ 1 ] = -m_rayEyeLeft.m_vec3Dir[ 1 ];
	m_rayEyeRight.m_vec3Origin[ 1 ] = -m_rayEyeRight.m_vec3Origin[ 1 ];
	m_rayEyeRight.m_vec3Dir[ 1 ] = -m_rayEyeRight.m_vec3Dir[ 1 ];
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
	
	printf( "%s\n", ToString( ).c_str( ) );
	printf( "Min: %s\n", vec3Min.ToString( 2 ).c_str( ) );

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

	printf( "%s\n", ToString( ).c_str( ) );
	printf( "Max: %f\n", std::max( vec3Max[ 0 ], vec3Max[ 1 ] ) );
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
	m_avec3Monitor{ CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ) },
	m_vec3Camera( CVector<3>( { 0, 0, 0 } ) ),
	m_avec3Face{ CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ) },
	m_rayEyeLeft( CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ) ),
	m_rayEyeRight( CVector<3>( { 0, 0, 0 } ), CVector<3>( { 0, 0, 0 } ) )
{

}

#define LINE( startp, endp, name )	CLine( img, startp[ 0 ], startp[ 1 ], endp[ 0 ], endp[ 1 ], "Line_" name ).Draw( Scalar( 255, 0, 0 ), 2, 8, 0, 0 );
void CScenery::Draw( CImage &img ) const
{
	//Draw monitor frame
	LINE( m_avec3Monitor[ 0 ], m_avec3Monitor[ 1 ], "Mon_LF" );
	LINE( m_avec3Monitor[ 1 ], m_avec3Monitor[ 2 ], "Mon_BF" );
	LINE( m_avec3Monitor[ 2 ], m_avec3Monitor[ 3 ], "Mon_RF" );
	LINE( m_avec3Monitor[ 3 ], m_avec3Monitor[ 0 ], "Mon_TF" );

	/*
	LINE( m_avec3Monitor[ 0 ], m_avec3Monitor[ 4 ], "Mon_LC" );
	LINE( m_avec3Monitor[ 1 ], m_avec3Monitor[ 5 ], "Mon_BC" );
	LINE( m_avec3Monitor[ 2 ], m_avec3Monitor[ 6 ], "Mon_RC" );
	LINE( m_avec3Monitor[ 3 ], m_avec3Monitor[ 7 ], "Mon_TC" );
	
	/*
	LINE( m_avec3Monitor[ 4 ], m_avec3Monitor[ 5 ], "Mon_LB" );
	LINE( m_avec3Monitor[ 5 ], m_avec3Monitor[ 6 ], "Mon_BB" );
	LINE( m_avec3Monitor[ 6 ], m_avec3Monitor[ 7 ], "Mon_RB" );
	LINE( m_avec3Monitor[ 7 ], m_avec3Monitor[ 4 ], "Mon_TB" );
	*/

	/*
	//Draw face frame
	LINE( m_avec3Face[ 0 ], m_avec3Face[ 1 ], "Face_LF" );
	LINE( m_avec3Face[ 1 ], m_avec3Face[ 2 ], "Face_BF" );
	LINE( m_avec3Face[ 2 ], m_avec3Face[ 3 ], "Face_RF" );
	LINE( m_avec3Face[ 3 ], m_avec3Face[ 0 ], "Face_TF" );

	LINE( m_avec3Face[ 0 ], m_avec3Face[ 4 ], "Face_LC" );
	LINE( m_avec3Face[ 1 ], m_avec3Face[ 5 ], "Face_BC" );
	LINE( m_avec3Face[ 2 ], m_avec3Face[ 6 ], "Face_RC" );
	LINE( m_avec3Face[ 3 ], m_avec3Face[ 7 ], "Face_TC" );

	LINE( m_avec3Face[ 4 ], m_avec3Face[ 5 ], "Face_LB" );
	LINE( m_avec3Face[ 5 ], m_avec3Face[ 6 ], "Face_BB" );
	LINE( m_avec3Face[ 6 ], m_avec3Face[ 7 ], "Face_RB" );
	LINE( m_avec3Face[ 7 ], m_avec3Face[ 4 ], "Face_TB" );
	*/

	//Draw gaze
	//CArrowedLine( img, m_rayEyeLeft.m_vec3Dir[ 0 ], m_rayEyeLeft.m_vec3Dir[ 1 ], m_rayEyeLeft( 1 )[ 0 ], m_rayEyeLeft( 1 )[ 1 ], "ArrowedLine_EyeLeft" ).Draw( Scalar( 0, 255, 0 ), 2, 8, 0, 0.1, 0 );
	//CArrowedLine( img, m_rayEyeRight.m_vec3Dir[ 0 ], m_rayEyeRight.m_vec3Dir[ 1 ], m_rayEyeRight( 1 )[ 0 ], m_rayEyeRight( 1 )[ 1 ], "ArrowedLine_EyeRight" ).Draw( Scalar( 0, 255, 0 ), 2, 8, 0, 0.1, 0 );

	//Draw eyes
	CPoint( img, m_rayEyeLeft.m_vec3Origin[ 0 ], m_rayEyeLeft.m_vec3Origin[ 1 ], "Point_EyeLeft" ).Draw( Scalar( 0, 255, 255 ), 1, -1, 0 );
	CPoint( img, m_rayEyeRight.m_vec3Origin[ 0 ], m_rayEyeRight.m_vec3Origin[ 1 ], "Point_EyeRight" ).Draw( Scalar( 0, 255, 255 ), 1, -1, 0 );

	//Draw camera
	CPoint( img, m_vec3Camera[ 0 ], m_vec3Camera[ 1 ], "Point_Camera" ).Draw( Scalar( 0, 255, 255 ), 1, -1, 0 );
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