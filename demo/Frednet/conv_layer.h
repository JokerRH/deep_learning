#pragma once

#include "image_layer.h"
#include "caffe_parameter_parser.hpp"

// debug printing
#include <iostream>
using namespace std;

// DEFINITION
template <class Dtype>
class conv_layer : public image_layer<Dtype>
{
public:
	struct layerparam
	{
		inline layerparam( const array4D<Dtype> &weights, const array1D<Dtype> &bias ) :
			weights( weights ),
			bias( bias )
		{

		}

		int numOutput;
		int kernelSize;
		int stride;
		int padding;
		std::string layerName = "";

		array4D<Dtype> weights;
		array1D<Dtype> bias;
	};

	conv_layer( const layerparam &lp, const array3D<Dtype> &inputData );
	conv_layer( const layerparam &lp, const image_layer<Dtype> &parentLayer );
	~conv_layer( void ) = default override;

	void printLayerParam( void );
	void forward( void ) override;

private:
	static constexpr std::array<unsigned, 3> CalcShape( const std::array<unsigned, 3> &auDim, const layerparam &lp );

	int numOutputs;
	int kernelSize;
	int stride;
	int padding;
	array4D<Dtype> convWeights;
	array1D<Dtype> bias;
};

// IMPLEMENTATION
template <class Dtype>
inline conv_layer<Dtype>::conv_layer( const conv_layer::layerParam &lp, array3D<Dtype> &inputData ) :
	base_layer( lp, inputData, CalcShape( inputData.auDim, lp ), lp.layerName ),
	convWeights( lp.weights ),
	bias( lp.bias ),
	numOutputs( lp.numOutputs ),
	kernelSize( lp.kernelSize ),
	stride( lp.stride ),
	padding( lp.padding ),
	layerName( lp.layerName )
{

}

template <class Dtype>
inline conv_layer<Dtype>::conv_layer( const conv_layer::layerparam &lp, base_layer<Dtype> &parentLayer ) :
	conv_layer( lp, parentLayer.getOutput( ) )
{

}

// printing private members
template <class Dtype>
inline void conv_layer<Dtype>::printLayerParam()
{
	cout << "Layer Name: " << this->layerName << endl;
	cout << "KernelSize: " << this->kernelSize << endl;
	cout << "NumOutputs: " << this->numOutputs << endl;
	cout << "Padding: " << this->padding << endl;
	cout << "Stride: " << this->stride << endl;
	cout << "Bias Adress: 0x" << this->bias.data << endl;
	cout << "Weights Address: 0x" << this->convWeights.data << endl;
	cout << "InputData Address: 0x" << this->inputData.data << endl;
	cout << "OutputData Address: 0x" << this->outputData.data << endl;
	cout << "Object Address: 0x" << this << endl << endl;

}

// doing forward path (convolution)
//*******************************************************************************
//TODO: pictureSize has to be changed for unsymetric Pictures
//TEMP: Check Picture Size: has to be changed with different Kernel Sizes
//		(line 109/111)
//		
// DONE: Adding Bias Addition
//		 Check passing pictureDepth and Picture Size via parameter(line 100 - 102)
//*******************************************************************************

template <class Dtype>
inline void conv_layer<Dtype>::forward()
{
	
	// Parameters read from Layer parameter struct
	int stride = this->stride;
	int kernelSize = this->kernelSize;
	int filterNum = this->numOutputs;
	int pictureSize = inputData.auDim[ 1 ]; // has to be changed for asymetric input!!!!!!!!!!
	int pictureDepth = inputData.auDim[ 2 ];
	//int filterDepth = pictureDepth; // mandatory for CNN convultion layers
	
	// Temporary calculation of pictureSize subtraction value -> needs to be veriefied
	int correctionFactorX = pictureSize - ( (pictureSize - kernelSize + (2 * this->padding)) / ((stride)) + 1);
	int correctionFactorY = pictureSize - ((pictureSize - kernelSize + (2 * this->padding)) / ((stride)) + 1);
	//cout << "Korrektur Faktor: " << correctionFactorX << endl;

	float oneDepth = 0; // cummulative value of one 2D Convolution
	float allDepth = 0; // cummulative value of all 2D Convolutions in Depth

	for (int filterNr = 0; filterNr < filterNum; filterNr++) {

		for (int picturey = 0; picturey < pictureSize - correctionFactorY; picturey += stride) { // pictureSize -1 or -2 depends on KernelSize -3 for real Conv

			for (int picturex = 0; picturex < pictureSize - correctionFactorX; picturex += stride) {  // pictureSize -1 or -2 depends on KernelSize  -3 for real Conv

				for (int depth = 0; depth < pictureDepth; depth++) {

					//cout << "Filter Depth: " << depth << endl;

					for (int filterx = 0; filterx < kernelSize; filterx++) { // first moving filter down ( y-Direction ) [][Y] 

						 for (int filtery = 0; filtery < kernelSize; filtery++) {

							int xn = picturex + filtery; // coordinates picture where the filter "lays" on 
							int yn = picturey + filterx;

							oneDepth = oneDepth + (convWeights[filterx][filtery][depth][filterNr] * inputData[xn][yn][depth]);
							//cout << convWeights[filterx][filtery][depth][filterNr] << " " << inputData[xn][yn][depth] << endl;
							//cout << "Filter X Koord. " << " Filter y Koord. " << " Pic. pos. x " << " Pic. pos. y " << " Depth " << " Filter Nr." << endl;
							//cout << "        " << filterx << "                " << filtery << "               " << xn << "        " << yn << "          " << depth << "     " << filterNr << endl;
							//cout << "Weight Value: " << convWeights[filterx][filtery][depth][filter] << " Picture Value: " << inputData[xn][yn][depth] << endl;
							//cout << "Temp Value For 2D: " << oneDepth << endl;

						}

					}//after 2D conv loop in Depth Loop
					allDepth = allDepth + oneDepth;
					oneDepth = 0;
				}
				//cout << "Temp Value For all Depth: " << allDepth << endl << endl;
				//cout << "Temp Value + Bias: " << allDepth << " + " << bias[filterNr] << endl;

				// Adding Bias to Output Value ( one Bias for each filter (neuron) )
				//cout << "Output Value: " << allDepth + bias[filterNr] << endl;
				//cout << "Output Data Position: " << picturex << " " << picturey << " " << filterNr << endl << endl;
				// writing convolution result to outputArray
				//if ((allDepth + bias[filterNr]) < 0) cout << "#####################################################################################################" << endl << endl << endl << endl << endl;
				outputData[picturex][picturey][filterNr] = allDepth+bias[filterNr]; // maybe wrong with bigger Stride
				allDepth = 0;
			}
		}
	}
}

template<class Dtype>
inline constexpr std::array<unsigned, 3> conv_layer<Dtype>::CalcShape( const std::array<unsigned, 3> &auDim, const layerparam &lp )
{
	return std::array<unsigned, 3>(
	{
		(unsigned) ( ( auDim[ 0 ] - lp.kernelSize + ( 2 * lp.padding ) ) / (Dtype) lp.stride + 1 ),
		(unsigned) ( ( auDim[ 1 ] - lp.kernelSize + ( 2 * lp.padding ) ) / (Dtype) lp.stride + 1 ),
		lp.numOutput
	} );
}