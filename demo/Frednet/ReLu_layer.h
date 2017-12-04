#ifndef RELU_LAYER_H
#define RELU_LAYER_H

#include "base_layer.h"


template <class type>
class relu_layer :
	public base_layer<type>
{
public:
	struct layerparam
	{
		std::string layerName = "";
	};

	relu_layer( const layerparam &lp, array3D<type> &inputData );
	relu_layer( const layerparam &lp, base_layer<type> &parentLayer );

	~relu_layer() override;
	void forward();
};


// IMPLEMENTATION

template <class type>
relu_layer<type>::relu_layer()
{

}


template<class type>
inline relu_layer<type>::relu_layer( const layerparam &lp, array3D<type> &inputData ) :
	base_layer( inputData, outputShape( inputData.aiDim[ 0 ], inputData.aiDim[ 1 ], inputData.aiDim[ 2 ] ), lp.layerName )
{

}

template<class type>
inline relu_layer<type>::relu_layer( const layerparam &lp, base_layer<type> &parentLayer ) :
	relu_layer( lp, parentLayer.getInput( ) )
{

}

// doing forward path (ReLu)
//*******************************************************************************
// TODO:
// DONE: Yet hardcoded width, height, depht should be somehow passed to object
//
//*******************************************************************************

template<class type>
void relu_layer<type>::forward()
{

	int inputWidth = inputData.aiDim[ 0 ];
	int inputHeight = inputData.aiDim[ 1 ];
	int inputDepth = inputData.aiDim[ 2 ];

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


template <class type>
relu_layer<type>::~relu_layer()
{
}



#endif
