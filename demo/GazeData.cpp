#include "GazeData.h"
#include "Render/Matrix.h"
#include "Render/Transformation.h"
#include <iostream>

CGazeData::CGazeData( const CData &data ) :
	CData( data ),
	rayEyeLeft( data.vec3EyeLeft, data.vec3GazePoint - data.vec3EyeLeft ),
	rayEyeRight( data.vec3EyeRight, data.vec3GazePoint - data.vec3EyeRight )
{

}

std::string CGazeData::ToString( unsigned uPrecision ) const
{
	std::ostringstream out;
	out.setf( std::ios_base::fixed, std::ios_base::floatfield );
	out.precision( uPrecision );

	std::wcout.setf( std::ios_base::fixed, std::ios_base::floatfield );
	std::wcout.precision( 6 );

#ifdef EXPORT_LOCAL
	CTransformation matTransform( GetFaceTransformation( ).Invert( ) );
#else
	CTransformation matTransform = CTransformation::GetRotationMatrix( CVector<3>( { 1, 0, 0 } ), CVector<3>( { 0, 1, 0 } ), CVector<3>( { 0, 0, -1 } ) );
#endif
	CVector<2> vec2EyeLeft( ( matTransform * rayEyeLeft ).AmplitudeRepresentation( ) );
	CVector<2> vec2EyeRight( ( matTransform * rayEyeRight ).AmplitudeRepresentation( ) );

	out << vec2EyeLeft[ 0 ] << "," << vec2EyeLeft[ 1 ] << "," << vec2EyeRight[ 0 ] << "," << vec2EyeRight[ 1 ] << ",";
	out << (double) ptEyeLeft.x / rectFace.width << "," << (double) ptEyeLeft.y / rectFace.height << ",";
	out << (double) ptEyeRight.x / rectFace.width << "," << (double) ptEyeRight.y / rectFace.height;
	return out.str( );
}