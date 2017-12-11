#pragma once

#include "base_layer.h"

template<class Dtype>
class image_layer : public base_layer<Dtype>
{
public:
	image_layer( const array3D<Dtype> &inputData, const std::array<unsigned, 3> &outputShape, const std::string &sLayerName );
	virtual ~image_layer( void ) = 0 override;

	const array3D<Dtype> &getInput( void ) const;
	const array3D<Dtype> &getOutput( void ) const;
	array3D<Dtype> &getOutput( void );

protected:
	const array3D<Dtype> &inputData;
	array3D<Dtype> outputData;
};

template<class Dtype>
inline image_layer<Dtype>::image_layer( const array3D<Dtype> &inputData, const std::array<unsigned, 3> &outputShape, const std::string &sLayerName ) :
	inputData( inputData ),
	outputData( outputShape ),
	m_sLayerName( sLayerName )
{

}

template<class Dtype>
inline constexpr const array3D<Dtype> &image_layer<Dtype>::getInput( void ) const
{
	return inputData;
}

template<class Dtype>
inline constexpr const array3D<Dtype> &image_layer<Dtype>::getOutput( void ) const
{
	return outputData
}

template<class Dtype>
inline constexpr array3D<Dtype> &image_layer<Dtype>::getOutput( void )
{
	return outputData;
}