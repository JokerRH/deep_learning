#include "Scenery.h"
#include "Line.h"
#include "ArrowedLine.h"
#include "Point.h"
#include "Render/RenderHelper.h"
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
	m_Camera( CVector<3>( { 0 } ) ),
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
	//Transform and scale Face
	CVector<3> vec3EyesX = m_rayEyeRight.m_vec3Origin - m_rayEyeLeft.m_vec3Origin;
	CVector<3> vec3EyesZ( { 0, 0, -1 } );
	CVector<3> vec3EyesY = vec3EyesX.CrossProduct( vec3EyesZ );
	CVector<3> vec3Offset = ( m_rayEyeRight.m_vec3Origin + m_rayEyeLeft.m_vec3Origin ) / 2;
	CMatrix<3, 3> matTransform = CRenderHelper::GetTransformationMatrix( vec3EyesX, vec3EyesY, vec3EyesZ ) * CRenderHelper::GetTransformationMatrix( vec3EyesX.Abs( ) );
	for( unsigned int u = 0; u < 8; u++ )
	{
		//m_avec3Face[ u ] *= dScale;
		m_avec3Face[ u ] = matTransform * m_avec3Face[ u ];
		m_avec3Face[ u ] += vec3Offset;
	}
	
	m_Face.Transform( matTransform );
	m_Face.Shift( vec3Offset );
}

CScenery CScenery::Transformed( const CMatrix<3, 3> &matTransform ) const
{
	CScenery scenery( *this );
	for( unsigned int u = 0; u < 8; u++ )
	{
		scenery.m_avec3Monitor[ u ] = matTransform * m_avec3Monitor[ u ];
		scenery.m_avec3Face[ u ] = matTransform * m_avec3Face[ u ];
	}

	scenery.m_vec3Camera = matTransform * m_vec3Camera;
	
	scenery.m_Monitor.Transform( matTransform );
	scenery.m_Face.Transform( matTransform );
	scenery.m_Camera.Transform( matTransform );
	scenery.m_rayEyeLeft.Transform( matTransform );
	scenery.m_rayEyeRight.Transform( matTransform );
	return scenery;
}

CScenery &CScenery::Transform( const CMatrix<3, 3> &matTransform )
{
	for( unsigned int u = 0; u < 8; u++ )
	{
		m_avec3Monitor[ u ] = matTransform * m_avec3Monitor[ u ];
		m_avec3Face[ u ] = matTransform * m_avec3Face[ u ];
	}

	m_vec3Camera = matTransform * m_vec3Camera;
	
	m_Monitor.Transform( matTransform );
	m_Face.Transform( matTransform );
	m_Camera.Transform( matTransform );
	m_rayEyeLeft.Transform( matTransform );
	m_rayEyeRight.Transform( matTransform );
	return *this;
}

CScenery &CScenery::Fit( void )
{
	CVector<3> vec3Shift = -CRenderObject::GetMin( { &m_Monitor, &m_Face, &m_Camera } );
	m_Monitor.Shift( vec3Shift );
	m_Face.Shift( vec3Shift );
	m_Camera.Shift( vec3Shift );
	m_rayEyeLeft.Shift( vec3Shift );
	m_rayEyeRight.Shift( vec3Shift );

	CVector<3> vec3Dim = CRenderObject::GetDim( { &m_Monitor, &m_Face, &m_Camera } );
	return Transform( CRenderHelper::GetTransformationMatrix( 1 / std::max( vec3Dim[ 0 ], vec3Dim[ 1 ] ) ) );
}

void CScenery::Draw( CImage &img ) const
{
	m_Monitor.GetPlane( CRenderBox::BOX_FRONT ).RenderContent( img, Scalar( 127, 0, 0 ) );
	m_Face.GetPlane( CRenderBox::BOX_FRONT ).RenderContent( img, Scalar( 0, 0, 127 ) );
	
	m_Monitor.RenderFrame( img, Scalar( 255, 0, 0 ) );
	m_Face.RenderFrame( img, Scalar( 0, 0, 255 ) );
	m_Camera.RenderPoints( img, Scalar( 255, 0, 255 ), 3 );
	m_rayEyeLeft.Render( img, Scalar( 0, 255, 255 ), Scalar( 0, 255, 0 ) );
	m_rayEyeRight.Render( img, Scalar( 0, 255, 255 ), Scalar( 0, 255, 0 ) );
	
	//Get gaze point
	CVector<2> vec2Gaze = m_rayEyeLeft.PointOfShortestDistance( m_rayEyeRight );
	CVector<3> vec3Gaze = ( m_rayEyeLeft( vec2Gaze[ 0 ] ) + m_rayEyeRight( vec2Gaze[ 1 ] ) ) / 2;
	CRenderPoint( vec3Gaze ).RenderPoints( img, Scalar( 0, 0, 255 ), 3 );
}