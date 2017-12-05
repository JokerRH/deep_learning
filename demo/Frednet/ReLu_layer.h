#pragma once

#include "image_layer.h"

template<class Dtype>
class relu_layer : public image_layer<Dtype>
{
public:
	struct layerparam
	{
		std::string layerName = "";
	};

	relu_layer( const layerparam &lp, const array3D<Dtype> &inputData );
	relu_layer( const layerparam &lp, const image_layer<Dtype> &parentLayer );
	~relu_layer( void ) = default override;

	void forward( void ) override;
};

// IMPLEMENTATION
template<class Dtype>
inline relu_layer<Dtype>::relu_layer( const layerparam &lp, const array3D<Dtype> &inputData ) :
	base_layer( inputData, outputShape( inputData.auDim[ 0 ], inputData.auDim[ 1 ], inputData.auDim[ 2 ] ), lp.layerName )
{

}

template<class Dtype>
inline relu_layer<type>::relu_layer( const layerparam &lp, const image_layer<Dtype> &parentLayer ) :
	relu_layer( lp, parentLayer.getOutput( ) )
{

}

// doing forward path (ReLu)
//*******************************************************************************
// TODO:
// DONE: Yet hardcoded width, height, depht should be somehow passed to object
//
//*******************************************************************************

template<class Dtype>
void relu_layer<Dtype>::forward()
{
	int inputWidth = inputData.auDim[ 0 ];
	int inputHeight = inputData.auDim[ 1 ];
	int inputDepth = inputData.auDim[ 2 ];

	for (int depthCount = 0; depthCount < inputDepth; depthCount++) {
		for (int widthCount = 0; widthCount < inputWidth; widthCount++) {
			for (int heightCount = 0; heightCount < inputHeight; heightCount++) {

				if ((this->inputData[heightCount][widthCount][depthCount]) < 0)
					this->outputData[heightCount][widthCount][depthCount] = 0;
				else 
					this->outputData[heightCount][widthCount][depthCount] = this->inputData[heightCount][widthCount][depthCount];
				//cout << "Data Position: " << widthCount << " " << heightCount << " " << depthCount << endl;
				//cout << "Data: " << outputData[heightCount][widthCount][depthCount] << endl;
			}   
		}
	}

}