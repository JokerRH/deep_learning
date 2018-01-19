#pragma once

#include "base_layer.h"

template<class Dtype>
class base_flat_layer : public base_layer<Dtype>
{
public:
	base_flat_layer( unsigned numOutput, const std::string &sLayerName );
	virtual ~base_flat_layer( void ) override = default;

	constexpr const array1D<Dtype> &getOutput( void ) const;
	inline array1D<Dtype> &getOutputR( void )
	{
		return outputData;
	}

protected:
	array1D<Dtype> outputData;
};

template <class Dtype>
class flat_layer : public base_flat_layer<Dtype>
{
public:
	flat_layer( const array1D<Dtype> &inputData, unsigned numOutput, const std::string &sLayerName );
	virtual ~flat_layer( void ) override = default;

	constexpr const array1D<Dtype> &getInput( void ) const;

protected:
	const array1D<Dtype> &inputData;
};

template<class Dtype>
inline base_flat_layer<Dtype>::base_flat_layer( unsigned numOutput, const std::string &sLayerName ) :
	base_layer<Dtype>( sLayerName ),
	outputData( { numOutput } )
{
	std::cout << "Layer \"" << sLayerName << "\" -> (" << numOutput << ")" << std::endl;
}

template<class Dtype>
inline constexpr const array1D<Dtype> &base_flat_layer<Dtype>::getOutput( void ) const
{
	return outputData;
}

template<class Dtype>
inline flat_layer<Dtype>::flat_layer( const array1D<Dtype> &inputData, unsigned numOutput, const std::string &sLayerName ) :
	base_flat_layer<Dtype>( numOutput, sLayerName ),
	inputData( inputData )
{

}

template <class type>
inline constexpr const array1D<type> &flat_layer<type>::getInput( void ) const
{
	return inputData;
}
