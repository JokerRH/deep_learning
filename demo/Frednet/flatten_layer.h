#pragma once

#include "flat_layer.h"
#include "image_layer.h"

template<class Dtype>
struct layerparam_flatten
{
	std::string layerName = std::string( "" );
};

template<class Dtype>
class flatten_layer : public base_flat_layer<Dtype>
{
public:
	flatten_layer( const layerparam_flatten<Dtype> &lp, const array3D<Dtype> &inputData );
	flatten_layer( const layerparam_flatten<Dtype> &lp, const image_layer<Dtype> &parentLayer );
	~flatten_layer( void ) override = default;

	void forward( void ) override;

private:
	const array3D<Dtype> &inputData;
};

// IMPLEMENTATION
template<class Dtype>
inline flatten_layer<Dtype>::flatten_layer( const layerparam_flatten<Dtype> &lp, const array3D<Dtype> &inputData ) :
	base_flat_layer<Dtype>( inputData.auDim[ 0 ] * inputData.auDim[ 1 ] * inputData.auDim[ 2 ], lp.layerName ),
	inputData( inputData )
{

}

template<class Dtype>
inline flatten_layer<Dtype>::flatten_layer( const layerparam_flatten<Dtype> &lp, const image_layer<Dtype> &parentLayer ) :
	flatten_layer( lp, parentLayer.getOutput( ) )
{

}

// caffe flattening implementation: NxCxWxH H first then W and then C
// 1. first channels first rows first element
// 2. first channels first rows second element
// 3. .................
// 4. first channels second rows first element
// 5. .............
// 6. second channels first rows first element
template <class Dtype>
void flatten_layer<Dtype>::forward()
{
	int inputWidth = inputData.auDim[ 0 ];
	int inputHeight = inputData.auDim[ 1 ];
	int inputDepth = inputData.auDim[ 2 ];
	int count = 0;
	for (int depth = 0; depth < inputDepth; depth++) {
		for (int lines = 0; lines < inputWidth; lines++) {
			for (int row = 0; row < inputHeight; row++) {
		
				base_flat_layer<Dtype>::outputData[count] = inputData[lines][row][depth];
				//cout << "Data: " <<inputData[lines][row][depth] << endl;
				//cout << "Position: " << lines<< " " << row << " " <<depth << endl;
				//cout << "Data: " << outputData[count] << endl;
				//cout << "Position: " << count << endl;
				count++;
			}
		}
	}
}
