#include "Scenery.h"
#include "Render\RenderHelper.h"

CScenery::CScenery( const CData &data ) :
	m_Camera( CVector<3>( { 0 } ) ),
	m_Forward( CVector<3>( { 0 } ), CVector<3>( { 0, 0, data.vec3EyeLeft[ 2 ] } ), CVector<2>( { 0, 0 } ) ),
	m_GazePoint( data.vec3GazePoint ),
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
	m_GazeLeft( data.vec3EyeLeft, data.vec3GazePoint ),
	m_GazeRight( data.vec3EyeRight, data.vec3GazePoint )
{
	//Transform and scale Face
	CVector<3> vec3EyesX = m_GazeRight.m_vec3Origin - m_GazeLeft.m_vec3Origin;
	CVector<3> vec3EyesZ( { 0, 0, -1 } );
	CVector<3> vec3EyesY = vec3EyesX.CrossProduct( vec3EyesZ );
	CVector<3> vec3Offset = ( m_GazeRight.m_vec3Origin + m_GazeLeft.m_vec3Origin ) / 2;
	CMatrix<3, 3> matTransform = CRenderHelper::GetTransformationMatrix( vec3EyesX, vec3EyesY, vec3EyesZ ) * CRenderHelper::GetTransformationMatrix( vec3EyesX.Abs( ) );

	m_Face.Transform( matTransform );
	m_Face.Shift( vec3Offset );
}

CScenery CScenery::Transformed( const CMatrix<3, 3> &matTransform ) const
{
	CScenery scenery( *this );
	scenery.m_Camera.Transform( matTransform );
	scenery.m_Forward.Transform( matTransform );
	scenery.m_GazePoint.Transform( matTransform );
	scenery.m_Face.Transform( matTransform );
	scenery.m_GazeLeft.Transform( matTransform );
	scenery.m_GazeRight.Transform( matTransform );
	return scenery;
}

CScenery &CScenery::Transform( const CMatrix<3, 3> &matTransform )
{
	m_Camera.Transform( matTransform );
	m_Forward.Transform( matTransform );
	m_GazePoint.Transform( matTransform );
	m_Face.Transform( matTransform );
	m_GazeLeft.Transform( matTransform );
	m_GazeRight.Transform( matTransform );
	return *this;
}

CScenery &CScenery::Fit( void )
{
	CVector<3> vec3Shift = -CRenderObject::GetMin( { &m_Camera, &m_GazePoint, &m_Face } );
	m_Camera.Shift( vec3Shift );
	m_Forward.Shift( vec3Shift );
	m_GazePoint.Shift( vec3Shift );
	m_Face.Shift( vec3Shift );
	m_GazeLeft.Shift( vec3Shift );
	m_GazeRight.Shift( vec3Shift );

	CVector<3> vec3Dim = CRenderObject::GetDim( { &m_Camera, &m_GazePoint, &m_Face } );
	return Transform( CRenderHelper::GetTransformationMatrix( 1 / std::max( vec3Dim[ 0 ], vec3Dim[ 1 ] ) ) );
}

void CScenery::Draw( cv::Mat &matImage ) const
{
	matImage.setTo( cv::Scalar::all( 0 ) );
	m_Face.GetPlane( CRenderBox::BOX_FRONT ).RenderContent( matImage, cv::Scalar( 0, 0, 127 ) );
	m_Face.RenderFrame( matImage, cv::Scalar( 0, 0, 255 ) );
	m_GazePoint.RenderPoints( matImage, cv::Scalar( 0, 0, 255 ), 3 );
	m_Camera.RenderPoints( matImage, cv::Scalar( 255, 0, 255 ), 3 );
	m_GazeLeft.Render( matImage, cv::Scalar( 0, 255, 255 ), cv::Scalar( 0, 255, 0 ) );
	m_GazeRight.Render( matImage, cv::Scalar( 0, 255, 255 ), cv::Scalar( 0, 255, 0 ) );
}