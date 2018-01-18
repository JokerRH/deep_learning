#pragma once

#include "flat_layer.h"

template <class Dtype>
struct layerparam_fc
{
	inline layerparam_fc( const array2D<Dtype> &weights, const array1D<Dtype> &bias ) :
		weights( weights ),
		bias( bias )
	{

	}

	int numOutput;
	std::string layerName = "";

	array2D<Dtype> weights;
	array1D<Dtype> bias;
};

template <class Dtype>
class fc_layer : public flat_layer<Dtype>
{
public:
	fc_layer( const layerparam_fc<Dtype> &lp, const array1D<Dtype> &inputData );
	fc_layer( const layerparam_fc<Dtype> &lp, const base_flat_layer<Dtype> &parentLayer );
	~fc_layer( void ) override = default;

	void forward( void ) override;

private:
	array2D<Dtype> weights;
	array1D<Dtype> bias;
};

// IMPLEMENTATION
template<class Dtype>
inline fc_layer<Dtype>::fc_layer( const layerparam_fc<Dtype> &lp, const array1D<Dtype> &inputData ) :
	flat_layer<Dtype>( inputData, lp.numOutput, lp.layerName ),
	weights( lp.weights ),
	bias( lp.bias )
{

}

template<class Dtype>
inline fc_layer<Dtype>::fc_layer( const layerparam_fc<Dtype> &lp, const base_flat_layer<Dtype> &parentLayer ) :
	fc_layer( lp, parentLayer.getOutput( ) )
{

}

template <class Dtype>
void fc_layer<Dtype>::forward( void )
{
	for (unsigned outputCounter = 0; outputCounter < flat_layer<Dtype>::outputData.auDim[ 0 ]; outputCounter++) {

		for (unsigned inputCounter = 0; inputCounter < flat_layer<Dtype>::inputData.auDim[ 0 ]; inputCounter++) {
			//cout << outputCounter << " " << inputCounter << " " << inputCounter << " " << outputCounter << endl;
			flat_layer<Dtype>::outputData[outputCounter] += flat_layer<Dtype>::inputData[inputCounter] * weights[inputCounter][outputCounter];
			
		}
		flat_layer<Dtype>::outputData[outputCounter] = flat_layer<Dtype>::outputData[outputCounter] + bias[outputCounter];
		if (flat_layer<Dtype>::outputData[outputCounter] < 0)
			flat_layer<Dtype>::outputData[outputCounter] = 0;
		//cout << "Position: " << outputCounter << endl;
		//cout << "Data: " << outputData[outputCounter] << endl;
	}
}
