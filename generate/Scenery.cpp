#include "Scenery.h"
#include "Render\Transformation.h"
#include "GazeData.h"
#include <iostream>

CScenery::CScenery( const CData &data ) :
	m_Camera( CVector<3>( { 0 } ) ),
	m_Forward( CVector<3>( { 0 } ), CVector<3>( { 0, 0, data.vec3EyeLeft[ 2 ] } ) ),
	m_Up( CVector<3>( { 0 } ), CVector<3>( { 0, 0.1, 0 } ) ),
	m_Right( CVector<3>( { 0 } ), CVector<3>( { 0.1, 0, 0 } ) ),
	m_GazePoint( data.vec3GazePoint ),
	m_Face( { CVector<3>( { 0 } ) } ),
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
	
	CVector<3> vec3EyeLeft( { (double) ( data.rectFace.width - data.ptEyeLeft.x ), (double) ( data.ptEyeLeft.y ), 0 } );
	CVector<3> vec3EyeRight( { (double) ( data.rectFace.width - data.ptEyeRight.x ), (double) ( data.ptEyeRight.y ), 0 } );
	CVector<3> vec3Center( ( vec3EyeLeft + vec3EyeRight ) / 2.0 );
	CVector<3> vec3TL( { (double) ( data.rectFace.width ), 0, 0 } );
	CVector<3> vec3Dim( { (double) ( data.rectFace.width ), (double) ( data.rectFace.height ), (double) ( data.rectFace.width ) } );

	vec3TL -= vec3Center;
	double dScale = ( vec3EyeRight - vec3EyeLeft ).Abs( );
	vec3TL /= dScale;
	vec3Dim /= dScale;
	CVector<3> vec3Min( { vec3TL[ 0 ] - vec3Dim[ 0 ], vec3TL[ 1 ], vec3TL[ 2 ] - vec3Dim[ 2 ] } );
	m_Face = CRenderBox( vec3Min, vec3Min + vec3Dim );
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

CTransformation CScenery::GetTransformation( void ) const
{
	return CTransformation::GetTRSMatrix( m_Camera.GetMin( ), m_Right.m_vec3Dir.Normalized( ), m_Up.m_vec3Dir.Normalized( ), m_Forward.m_vec3Dir.Normalized( ), m_Up.m_vec3Dir.Abs( ) / 0.1 );
}