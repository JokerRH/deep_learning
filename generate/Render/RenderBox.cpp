#include "RenderBox.h"

CVector<3> CRenderBox::GetMin( void ) const
{
	return CVector<3>(
	{
		std::min( { m_avec3Points[ 0 ][ 0 ], m_avec3Points[ 1 ][ 0 ], m_avec3Points[ 2 ][ 0 ], m_avec3Points[ 3 ][ 0 ], m_avec3Points[ 4 ][ 0 ], m_avec3Points[ 5 ][ 0 ], m_avec3Points[ 6 ][ 0 ], m_avec3Points[ 7 ][ 0 ] } ),
		std::min( { m_avec3Points[ 0 ][ 1 ], m_avec3Points[ 1 ][ 1 ], m_avec3Points[ 2 ][ 1 ], m_avec3Points[ 3 ][ 1 ], m_avec3Points[ 4 ][ 1 ], m_avec3Points[ 5 ][ 1 ], m_avec3Points[ 6 ][ 1 ], m_avec3Points[ 7 ][ 1 ] } ),
		std::min( { m_avec3Points[ 0 ][ 2 ], m_avec3Points[ 1 ][ 2 ], m_avec3Points[ 2 ][ 2 ], m_avec3Points[ 3 ][ 2 ], m_avec3Points[ 4 ][ 2 ], m_avec3Points[ 5 ][ 2 ], m_avec3Points[ 6 ][ 2 ], m_avec3Points[ 7 ][ 2 ] } )
	} );
}

CVector<3> CRenderBox::GetMax( void ) const
{
	return CVector<3>(
	{
		std::max( { m_avec3Points[ 0 ][ 0 ], m_avec3Points[ 1 ][ 0 ], m_avec3Points[ 2 ][ 0 ], m_avec3Points[ 3 ][ 0 ], m_avec3Points[ 4 ][ 0 ], m_avec3Points[ 5 ][ 0 ], m_avec3Points[ 6 ][ 0 ], m_avec3Points[ 7 ][ 0 ] } ),
		std::max( { m_avec3Points[ 0 ][ 1 ], m_avec3Points[ 1 ][ 1 ], m_avec3Points[ 2 ][ 1 ], m_avec3Points[ 3 ][ 1 ], m_avec3Points[ 4 ][ 1 ], m_avec3Points[ 5 ][ 1 ], m_avec3Points[ 6 ][ 1 ], m_avec3Points[ 7 ][ 1 ] } ),
		std::max( { m_avec3Points[ 0 ][ 2 ], m_avec3Points[ 1 ][ 2 ], m_avec3Points[ 2 ][ 2 ], m_avec3Points[ 3 ][ 2 ], m_avec3Points[ 4 ][ 2 ], m_avec3Points[ 5 ][ 2 ], m_avec3Points[ 6 ][ 2 ], m_avec3Points[ 7 ][ 2 ] } )
	} );
}

CRenderBox::CRenderBox( const CVector<3> &vec3Min, const CVector<3> &vec3Max ) :
	m_avec3Points{ CVector<3>( { 0 } ), CVector<3>( { 0 } ), CVector<3>( { 0 } ), CVector<3>( { 0 } ), CVector<3>( { 0 } ), CVector<3>( { 0 } ), CVector<3>( { 0 } ), CVector<3>( { 0 } ) }
{
	const CVector<3> vec3DimX( { vec3Max[ 0 ] - vec3Min[ 0 ], 0, 0 } );
	const CVector<3> vec3DimY( { 0, vec3Max[ 1 ] - vec3Min[ 1 ], 0 } );
	const CVector<3> vec3DimZ( { 0, 0, vec3Max[ 2 ] - vec3Min[ 2 ] } );
	
	m_avec3Points[ 0 ] = vec3Max - vec3DimX;
	m_avec3Points[ 1 ] = vec3Min + vec3DimZ;
	m_avec3Points[ 2 ] = vec3Max - vec3DimY;
	m_avec3Points[ 3 ] = vec3Max;
	m_avec3Points[ 4 ] = vec3Min + vec3DimY;
	m_avec3Points[ 5 ] = vec3Min;
	m_avec3Points[ 6 ] = vec3Min + vec3DimX;
	m_avec3Points[ 6 ] = vec3Max - vec3DimZ;
}

#define BOX( plane, a, b, c, d )\
	if( fPlane & plane )\
	{\
		if( fLine & CRenderPlane::PLANE_LEFT )\
		{\
			abIndices[ 0 ] = a;\
			abIndices[ 1 ] = b;\
			fLine &= ~CRenderPlane::PLANE_LEFT;\
		}\
		else if( fLine & CRenderPlane::PLANE_BOTTOM )\
		{\
			abIndices[ 0 ] = b;\
			abIndices[ 1 ] = c;\
			fLine &= ~CRenderPlane::PLANE_BOTTOM;\
		}\
		else if( fLine & CRenderPlane::PLANE_RIGHT )\
		{\
			abIndices[ 0 ] = c;\
			abIndices[ 1 ] = d;\
			fLine &= ~CRenderPlane::PLANE_RIGHT;\
		}\
		else if( fLine & CRenderPlane::PLANE_TOP )\
		{\
			abIndices[ 0 ] = d;\
			abIndices[ 1 ] = a;\
			fLine &= ~CRenderPlane::PLANE_TOP;\
		}\
		else\
		{\
			fprintf( stderr, "Unknown line in CRenderBox::GetLineIndices\n" );\
			assert( false );\
		}\
		if( !fLine )\
			fPlane &= ~plane;\
	}

std::array<unsigned char, 2> CRenderBox::GetLineIndices( unsigned char &fPlane, unsigned char &fLine ) const
{
	std::array<unsigned char, 2> abIndices;
	BOX( BOX_FRONT, 0, 1, 2, 3 )
	else BOX( BOX_BACK, 7, 6, 5, 4 )
	else BOX( BOX_LEFT, 4, 5, 1, 0 )
	else BOX( BOX_RIGHT, 3, 2, 6, 7 )
	else BOX( BOX_TOP, 4, 0, 3, 7 )
	else BOX( BOX_BOTTOM, 1, 5, 6, 2 )
	else
	{
		fprintf( stderr, "Unknown plane in CRenderBox::GetLineIndices\n" );
		assert( false );
	}
	
	return abIndices;
}
#undef BOX

std::array<unsigned char, 4> CRenderBox::GetPlaneIndices( unsigned char &fPlane ) const
{
	std::array<unsigned char, 4> abIndices;
	if( fPlane & BOX_FRONT )
	{
		abIndices[ 0 ] = 0;
		abIndices[ 1 ] = 1;
		abIndices[ 2 ] = 2;
		abIndices[ 3 ] = 3;
		fPlane &= ~BOX_FRONT;
	}
	else if( fPlane & BOX_BACK )
	{
		abIndices[ 0 ] = 4;
		abIndices[ 1 ] = 5;
		abIndices[ 2 ] = 6;
		abIndices[ 3 ] = 7;
		fPlane &= ~BOX_BACK;
	}
	else if( fPlane & BOX_LEFT )
	{
		abIndices[ 0 ] = 0;
		abIndices[ 1 ] = 4;
		abIndices[ 2 ] = 5;
		abIndices[ 3 ] = 1;
		fPlane &= ~BOX_LEFT;
	}
	else if( fPlane & BOX_RIGHT )
	{
		abIndices[ 0 ] = 3;
		abIndices[ 1 ] = 2;
		abIndices[ 2 ] = 6;
		abIndices[ 3 ] = 7;
		fPlane &= ~BOX_RIGHT;
	}
	else if( fPlane & BOX_TOP )
	{
		abIndices[ 0 ] = 0;
		abIndices[ 1 ] = 3;
		abIndices[ 2 ] = 7;
		abIndices[ 3 ] = 4;
		fPlane &= ~BOX_TOP;
	}
	else if( fPlane & BOX_BOTTOM )
	{
		abIndices[ 0 ] = 1;
		abIndices[ 1 ] = 2;
		abIndices[ 2 ] = 6;
		abIndices[ 3 ] = 5;
		fPlane &= ~BOX_BOTTOM;
	}
	else
	{
		fprintf( stderr, "Unknown plane in CRenderBox::GetPlaneIndices\n" );
		assert( false );
	}

	return abIndices;
}