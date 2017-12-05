#pragma once

#include "image_layer.h"

template <class Dtype>
class concat_layer : public base_flat_layer<Dtype>
{
public:
	struct layerparam
	{
		std::string layerName = "";
	};

	concat_layer( const layerparam &lp, const array1D<Dtype> &inputData1, const array1D<Dtype> &inputData2 );
	concat_layer( const layerparam &lp, const base_flat_layer<Dtype> &parentLayer1, const base_flat_layer<Dtype> &parentLayer2 );
	~concat_layer( void ) = default override;

	void forward( void ) override;

private:
	const array1D<Dtype> &inputData1;
	const array1D<Dtype> &inputData2;
};

// IMPLEMENTATION
template<class Dtype>
inline concat_layer<Dtype>::concat_layer( const layerparam &lp, const array1D<Dtype> &inputData1, const array1D<Dtype> &inputData2 ) :
	base_flat_layer( { inputData1.auDim[ 0 ] + inputData2.auDim[ 0 ] }, lp.layerName ),
	inputData1( inputData1 ),
	inputData2( inputData2 )
{

}

template<class Dtype>
inline concat_layer<Dtype>::concat_layer( const layerparam &lp, const base_flat_layer<Dtype> &parentLayer1, const base_flat_layer<Dtype> &parentLayer2 ) :
	concat_layer( lp, parentLayer1.getOutput( ), parentLayer2.getOutput( ) )
{

}

template <class type>
void concat_layer<type>::forward( void )
{
	int handover = 0;

	for (int counter1 = 0; counter1 < inputData1.auDim[ 0 ]; counter1++) {
		outputData[counter1] = inputData1[counter1];
		handover++;
		//cout << "Data: " << outputData[counter1] << endl;
		//cout << "position: " << counter1 << endl;
	}

	for (int counter2 = 0; counter2 < inputData2.auDim[ 0 ]; counter2++) {
		outputData[counter2 + handover] = inputData2[counter2];
		//cout << "Data: " << outputData[counter2+handover] << endl;
		//cout << "position: " << counter2+handover << endl;
	}
}