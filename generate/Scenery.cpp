#include "Scenery.h"
#include "Render\Transformation.h"
#include "GazeData.h"

CScenery::CScenery( const CData &data ) :
	m_Camera( CVector<3>( { 0 } ) ),
	m_Forward( CVector<3>( { 0 } ), CVector<3>( { 0, 0, data.vec3EyeLeft[ 2 ] } ) ),
	m_Up( CVector<3>( { 0 } ), CVector<3>( { 0, 0.1, 0 } ) ),
	m_Right( CVector<3>( { 0 } ), CVector<3>( { 0.1, 0, 0 } ) ),
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
	m_GazeLeft( data.vec3EyeLeft, data.vec3GazePoint - data.vec3EyeLeft ),
	m_GazeRight( data.vec3EyeRight, data.vec3GazePoint - data.vec3EyeRight )
{
	//Check if data is GazeData
	try
	{
		const CGazeData gazedata = dynamic_cast<const CGazeData &>( data );
		m_GazeLeft = gazedata.rayEyeLeft;
		m_GazeRight = gazedata.rayEyeRight;
	}
	catch( std::bad_cast )
	{
		
	}
	
	//Transform and scale Face
	m_Face *= data.GetFaceTransformation( );
}

CScenery &CScenery::Fit( bool fShift )
{
	CVector<3> vec3Dim = CRenderObject::GetDim( { &m_Camera, &m_GazePoint, &m_Face } );
	CVector<3> vec3Min = CRenderObject::GetMin( { &m_Camera, &m_GazePoint, &m_Face } );
	CVector<3> vec3Translate( { 0 } );
	if( fShift )
		vec3Translate = -( vec3Min + vec3Dim * 0.5 );
	else
		vec3Dim = ( vec3Min + vec3Dim ) * 2.0;

	*this *= CTransformation::GetTranslationMatrix( vec3Translate ) * CTransformation::GetScaleMatrix( 1 / std::max( vec3Dim[ 0 ], vec3Dim[ 1 ] ) );
	return *this;
}

void CScenery::Draw( cv::Mat &matImage ) const
{
	matImage.setTo( cv::Scalar::all( 0 ) );
	CVector<3> vec3Shift( { 0.5, 0.5, 0 } );
	m_Face.GetPlane( CRenderBox::BOX_FRONT ).RenderContent( matImage, cv::Scalar( 0, 0, 127 ) );
	m_Forward.Render( matImage, cv::Scalar( 255, 0, 255 ), cv::Scalar( 0, 127, 0 ) );
	m_Up.Render( matImage, cv::Scalar( 255, 0, 255 ), cv::Scalar( 0, 127, 0 ) );
	m_Right.Render( matImage, cv::Scalar( 255, 0, 255 ), cv::Scalar( 0, 127, 127 ) );
	m_Face.RenderFrame( matImage, cv::Scalar( 0, 0, 255 ) );
	m_GazePoint.RenderPoints( matImage, cv::Scalar( 0, 0, 255 ), 3 );
	m_Camera.RenderPoints( matImage, cv::Scalar( 255, 0, 255 ), 3 );
	m_GazeLeft.Render( matImage, cv::Scalar( 0, 255, 255 ), cv::Scalar( 0, 255, 0 ) );
	m_GazeRight.Render( matImage, cv::Scalar( 0, 255, 255 ), cv::Scalar( 0, 255, 0 ) );
}

CScenery operator*( const CTransformation &matTransform, const CScenery &scenery )
{
	CScenery ret( scenery );
	ret.m_matTransform *= matTransform;
	ret.m_Camera *= matTransform;
	ret.m_Forward *= matTransform;
	ret.m_Up *= matTransform;
	ret.m_Right *= matTransform;
	ret.m_GazePoint *= matTransform;
	ret.m_Face *= matTransform;
	ret.m_GazeLeft *= matTransform;
	ret.m_GazeRight *= matTransform;
	return ret;
}

CScenery &CScenery::operator*=( const CTransformation &matTransform )
{
	m_matTransform *= matTransform;
	m_Camera *= matTransform;
	m_Forward *= matTransform;
	m_Up *= matTransform;
	m_Right *= matTransform;
	m_GazePoint *= matTransform;
	m_Face *= matTransform;
	m_GazeLeft *= matTransform;
	m_GazeRight *= matTransform;
	return *this;
}