#pragma once

#include "base_layer.h"

template<class Dtype>
class image_layer : public base_layer<Dtype>
{
public:
	image_layer( const array3D<Dtype> &inputData, const std::array<unsigned, 3> &outputShape, const std::string &sLayerName );
	virtual ~image_layer( void ) override = default;

	constexpr const array3D<Dtype> &getInput( void ) const;
	constexpr const array3D<Dtype> &getOutput( void ) const;
	inline array3D<Dtype> &getOutputR( void )
	{
		return outputData;
	}

protected:
	const array3D<Dtype> &inputData;
	array3D<Dtype> outputData;
};

template<class Dtype>
inline image_layer<Dtype>::image_layer( const array3D<Dtype> &inputData, const std::array<unsigned, 3> &outputShape, const std::string &sLayerName ) :
	base_layer<Dtype>( sLayerName ),
	inputData( inputData ),
	outputData( outputShape )
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
	return outputData;
}
