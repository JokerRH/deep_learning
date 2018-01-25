#pragma once

#include "image_layer.h"

template<class Dtype>
struct layerparam_relu
{
	std::string layerName = std::string( "" );
};

template<class Dtype>
class relu_layer : public image_layer<Dtype>
{
public:
	relu_layer( const layerparam_relu<Dtype> &lp, const array3D<Dtype> &inputData );
	relu_layer( const layerparam_relu<Dtype> &lp, const image_layer<Dtype> &parentLayer );
	~relu_layer( void ) override = default;

	void forward( void ) override;
};

// IMPLEMENTATION
template<class Dtype>
inline relu_layer<Dtype>::relu_layer( const layerparam_relu<Dtype> &lp, const array3D<Dtype> &inputData ) :
	image_layer<Dtype>( inputData, inputData.auDim, lp.layerName )
{

}

template<class Dtype>
inline relu_layer<Dtype>::relu_layer( const layerparam_relu<Dtype> &lp, const image_layer<Dtype> &parentLayer ) :
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
	std::cout << "\"" << image_layer<Dtype>::m_sLayerName << "\" running" << std::endl;
	int inputWidth = image_layer<Dtype>::inputData.auDim[ 0 ];
	int inputHeight = image_layer<Dtype>::inputData.auDim[ 1 ];
	int inputDepth = image_layer<Dtype>::inputData.auDim[ 2 ];

	for (int depthCount = 0; depthCount < inputDepth; depthCount++) {
		for (int widthCount = 0; widthCount < inputWidth; widthCount++) {
			for (int heightCount = 0; heightCount < inputHeight; heightCount++) {

				if ((this->image_layer<Dtype>::inputData[heightCount][widthCount][depthCount]) < 0)
					this->image_layer<Dtype>::outputData[heightCount][widthCount][depthCount] = 0;
				else 
					this->image_layer<Dtype>::outputData[heightCount][widthCount][depthCount] = this->image_layer<Dtype>::inputData[heightCount][widthCount][depthCount];
				//cout << "Data Position: " << widthCount << " " << heightCount << " " << depthCount << endl;
				//cout << "Data: " << outputData[heightCount][widthCount][depthCount] << endl;
			}   
		}
	}

}
