#pragma once

#include "dynamic_arrays.h"

template <class Dtype>
class base_layer
{
public:
	base_layer( const std::string &sLayerName );
	virtual ~base_layer( void ) = default;

	virtual void forward( void ) = 0;

protected:
	const std::string m_sLayerName;
};

template <class Dtype>
inline base_layer<Dtype>::base_layer( const std::string &sLayerName ) :
	m_sLayerName( sLayerName )
{

}