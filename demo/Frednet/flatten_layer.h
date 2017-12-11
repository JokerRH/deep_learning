#pragma once

#include "flat_layer.h"
#include "image_layer.h"

template<class Dtype>
class flatten_layer : public base_flat_layer<Dtype>
{
public:
	struct layerparam
	{
		std::string layerName = "";
	};

	flatten_layer( const layerparam &lp, const array3D<Dtype> &inputData );
	flatten_layer( const layerparam &lp, const image_layer<Dtype> &parentLayer );
	~flatten_layer( void ) = default override;

	void forward( void ) override;

private:
	const array3D<Dtype> &inputData;
};

// IMPLEMENTATION
template<class type>
inline flatten_layer<type>::flatten_layer( const layerparam &lp, const array3D<type> &inputData ) :
	base_flat_layer( inputData.auDim[ 0 ] * inputData.auDim[ 1 ] * inputData.auDim[ 2 ], lp.layerName ),
	inputData( inputData )
{

}

template<class type>
inline flatten_layer<type>::flatten_layer( const layerparam &lp, const image_layer<type> &parentLayer ) :
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
template <class type>
void flatten_layer<type>::forward()
{
	int inputWidth = inputData.auDim[ 0 ];
	int inputHeight = inputData.auDim[ 1 ];
	int inputDepth = inputData.auDim[ 2 ];
	int count = 0;
	for (int depth = 0; depth < inputDepth; depth++) {
		for (int lines = 0; lines < inputWidth; lines++) {
			for (int row = 0; row < inputHeight; row++) {
		
				outputData[count] = inputData[lines][row][depth];
				//cout << "Data: " <<inputData[lines][row][depth] << endl;
				//cout << "Position: " << lines<< " " << row << " " <<depth << endl;
				//cout << "Data: " << outputData[count] << endl;
				//cout << "Position: " << count << endl;
				count++;
			}
		}
	}
}