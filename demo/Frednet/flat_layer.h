#pragma once

#include "base_layer.h"

template<class Dtype>
class base_flat_layer : public base_layer<Dtype>
{
public:
	base_flat_layer( unsigned numOutput, const std::string &sLayerName );
	virtual ~base_flat_layer( void ) = 0 override;

	constexpr const array1D<Dtype> &getOutput( void ) const;
	constexpr array1D<Dtype> &getOutputR( void );

protected:
	array1D<Dtype> outputData;
};

template <class Dtype>
class flat_layer : public base_flat_layer<Dtype>
{
public:
	flat_layer( const array1D<Dtype> &inputData, unsigned numOutput, const std::string &sLayerName );
	virtual ~flat_layer( void ) = 0 override;

	constexpr const array1D<Dtype> &getInput( void ) const;

protected:
	const array1D<Dtype> &inputData;
};

template<class type>
inline base_flat_layer<type>::base_flat_layer( unsigned numOutput, const std::string &sLayerName ) :
	base_layer( sLayerName ),
	outputData( { numOutput } )
{

}

template<class Dtype>
inline constexpr const array1D<Dtype> &base_flat_layer<Dtype>::getOutput( void ) const
{
	return outputData;
}

template<class Dtype>
inline constexpr array1D<Dtype> &base_flat_layer<Dtype>::getOutputR( void )
{
	return outputData;
}

template<class type>
inline flat_layer<type>::flat_layer( const array1D<type> &inputData, unsigned numOutput, const std::string &sLayerName ) :
	base_flat_layer( numOutput, sLayerName ),
	inputData( inputData )
{

}

template <class type>
inline constexpr const array1D<type> &flat_layer<type>::getInput( void ) const
{
	return inputData;
}