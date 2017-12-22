#pragma once

#include "image_layer.h"

template <class Dtype>
struct layerparam_mvn
{
	std::string layerName = "";
};

template <class Dtype>
class mvn_layer : public image_layer<Dtype>
{
public:
	mvn_layer( const layerparam_mvn<Dtype> &lp, const array3D<Dtype> &inputData );
	mvn_layer( const layerparam_mvn<Dtype> &lp, const image_layer<Dtype> &parentLayer );
	~mvn_layer( void ) override = default;

	void forward( void ) override;
};

// IMPLEMENTATION
template<class Dtype>
inline mvn_layer<Dtype>::mvn_layer( const layerparam_mvn<Dtype> &lp, const array3D<Dtype> &inputData ) :
	image_layer( inputData, inputData.auDim, lp.layerName )
{

}

template<class Dtype>
inline mvn_layer<Dtype>::mvn_layer( const layerparam_mvn<Dtype> &lp, const image_layer<Dtype> &parentLayer ) :
	mvn_layer( lp, parentLayer.getOutput( ) )
{

}

template<class Dtype>
inline void mvn_layer<Dtype>::forward( void )
{
	outputData.copy( inputData );
	for( unsigned uZ = 0; uZ < outputData.auDim[ 2 ]; uZ++ )
	{
		//Calculate mean
		Dtype rMean = 0;
		for( unsigned uX = 0; uX < outputData.auDim[ 0 ]; uX++ )
			for( unsigned uY = 0; uY < outputData.auDim[ 1 ]; uY++ )
				rMean += outputData[ uX ][ uY ][ uZ ];

		rMean /= outputData.auDim[ 0 ] * outputData.auDim[ 1 ];

		//Subtract mean
		for( unsigned uX = 0; uX < outputData.auDim[ 0 ]; uX++ )
			for( unsigned uY = 0; uY < outputData.auDim[ 1 ]; uY++ )
				outputData[ uX ][ uY ][ uZ ] -= rMean;

		//normalize variance
		for( unsigned uX = 0; uX < outputData.auDim[ 0 ]; uX++ )
			for( unsigned uY = 0; uY < outputData.auDim[ 1 ]; uY++ )
			{
				Dtype rVar = outputData[ uX ][ uY ][ uZ ] - rMean;
				rVar *= rVar;
				outputData[ uX ][ uY ][ uZ ] /= (Dtype) ( rVar + ( 1e-9 ) );
			}
	}
}