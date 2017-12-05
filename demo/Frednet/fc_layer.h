#pragma once

#include "flat_layer.h"

template <class Dtype>
class fc_layer : public flat_layer<Dtype>
{
public:
	struct layerparam
	{
		inline layerparam( const array2D<Dtype> &weights, const array1D<Dtype> &bias ) :
			weights( weights ),
			bias( bias )
		{

		}

		int numOutput;
		std::string layerName = "";

		array4D<type> weights;
		array1D<type> bias;
	};

	fc_layer( const layerparam &lp, const array1D<type> &inputData );
	fc_layer( const layerparam &lp, const base_flat_layer<type> &parentLayer );
	~fc_layer( void ) = default override;

	void forward( void ) override;

private:
	array2D<Dtype> weights;
	array1D<Dtype> bias;
};

// IMPLEMENTATION
template<class Dtype>
inline fc_layer<Dtype>::fc_layer( const layerparam &lp, const array1D<Dtype> &inputData ) :
	flat_layer( inputData, lp.numOutput, lp.layerName ),
	weights( lp.weights ),
	bias( lp.bias )
{

}

template<class Dtype>
inline fc_layer<Dtype>::fc_layer( const layerparam &lp, const base_flat_layer<Dtype> &parentLayer ) :
	fc_layer( lp, parentLayer.getOutput( ) )
{

}

template <class Dtype>
void fc_layer<Dtype>::forward( void )
{
	for (int outputCounter = 0; outputCounter < numOutput; outputCounter++) {

		for (int inputCounter = 0; inputCounter < this->inputLenght; inputCounter++) {
			//cout << outputCounter << " " << inputCounter << " " << inputCounter << " " << outputCounter << endl;
			outputData[outputCounter] += inputData[inputCounter] * weights[inputCounter][outputCounter];
			
		}
		outputData[outputCounter] = outputData[outputCounter] + bias[outputCounter];
		if (outputData[outputCounter] < 0)
			outputData[outputCounter] = 0;
		//cout << "Position: " << outputCounter << endl;
		//cout << "Data: " << outputData[outputCounter] << endl;
	}
}