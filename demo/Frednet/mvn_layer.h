#pragma once

#include "image_layer.h"

template <class Dtype>
class mvn_layer : public image_layer<Dtype>
{
public:
	struct layerparam
	{
		std::string layerName = "";
	};

	mvn_layer( const layerparam &lp, const array3D<Dtype> &inputData );
	mvn_layer( const layerparam &lp, const image_layer<Dtype> &parentLayer );
	~mvn_layer( void ) = default override;

	void forward( void ) override;
};

// IMPLEMENTATION
template<class Dtype>
inline mvn_layer<Dtype>::mvn_layer( const layerparam &lp, const array3D<Dtype> &inputData ) :
	base_layer( inputData, outputShape( inputData.auDim[ 0 ], inputData.auDim[ 1 ], inputData.auDim[ 2 ] ), lp.layerName )
{

}

template<class Dtype>
inline mvn_layer<Dtype>::mvn_layer( const layerparam &lp, const image_layer<Dtype> &parentLayer ) :
	relu_layer( lp, parentLayer.getOutput( ) )
{

}

template<class Dtype>
inline void mvn_layer<Dtype>::forward( void )
{
	for( unsigned uZ = 0; uZ < inputData.auDim[ 2 ]; uZ++ )
	{
		//Calculate mean
		Dtype rMean = 0;
		for( unsigned uX = 0; uX < inputData.auDim[ 0 ]; uX++ )
			for( unsigned uY = 0; uY < inputData.auDim[ 1 ]; uY++ )
				rMean += inputData[ uX ][ uY ][ uZ ];

		rMean /= inputData.auDim[ 0 ] * inputData.auDim[ 1 ];

		//Subtract mean
		for( unsigned uX = 0; uX < inputData.auDim[ 0 ]; uX++ )
			for( unsigned uY = 0; uY < inputData.auDim[ 1 ]; uY++ )
				inputData[ uX ][ uY ][ uZ ] -= rMean;

		//normalize variance
		for( unsigned uX = 0; uX < inputData.auDim[ 0 ]; uX++ )
			for( unsigned uY = 0; uY < inputData.auDim[ 1 ]; uY++ )
			{
				T rVar = inputData[ uX ][ uY ][ uZ ] - rMean;
				rVar *= rVar;
				inputData[ uX ][ uY ][ uZ ] /= ( rVar + ( 1e-9 ) );
			}
	}
}