#pragma once

#include "image_layer.h"
#include <array>

template <class Dtype>
struct layerparam_pooling
{
	enum mode
	{
		MAX,
		AVE,
		STOCHASTIC
	};

	int kernelSize;
	int stride;
	mode pool;
	std::string layerName = "";
};

template <class Dtype>
class pooling_layer : public image_layer<Dtype>
{
public:
	pooling_layer( const layerparam_pooling<Dtype> &lp, const array3D<Dtype> &inputData );
	pooling_layer( const layerparam_pooling<Dtype> &lp, const image_layer<Dtype> &parentLayer );
	~pooling_layer( void ) override = default;

	void forward( void ) override;

private:
	static std::array<unsigned, 3> CalcShape( const std::array<unsigned, 3> &auDim, const layerparam_pooling<Dtype> &lp );

	int kernelSize;
	int stride;
	int pool;
};


// IMPLEMENTATION
template<class Dtype>
inline pooling_layer<Dtype>::pooling_layer( const layerparam_pooling<Dtype> &lp, const array3D<Dtype> &inputData ) :
	image_layer( inputData, CalcShape( inputData.auDim, lp ), lp.layerName ),
	kernelSize( lp.kernelSize ),
	stride( lp.stride ),
	pool( lp.pool )
{

}

template<class Dtype>
inline pooling_layer<Dtype>::pooling_layer( const layerparam_pooling<Dtype> &lp, const image_layer<Dtype> &parentLayer ) :
	pooling_layer( lp, parentLayer.getOutput( ) )
{

}

// doing forward path (pooling)
//*******************************************************************************
// TODO: PictureSize has to be changed for unsymetric input!!!!!!!!!!
// DONE:	Check passing pictureDepth and Picture Size via parameter
//
//*******************************************************************************
template <class Dtype>
void pooling_layer<Dtype>::forward( void )
{

	// Parameters read from Layer parameter struct
	int stride = this->stride;
	int pictureDepth = inputData.auDim[ 2 ];
	int pictureSize = inputData.auDim[ 1 ];   // has to be changed for unsymetric input!!!!!!!!!!
	Dtype tempMax = 0;
	int kernelSize = this->kernelSize;

	//int correctionFactorX = pictureSize - ((pictureSize - kernelSize) / (stride)+1);
	//int correctionFactorY = pictureSize- ((pictureSize - kernelSize) / (stride)+1);
	//cout << "Korrektur Faktor: " << correctionFactorX << endl;


	int outputX = 0; // necessary for finding right output position if stride is not equal to 1 ( maybe necessary in Conv)
	int outputY = 0;

	for (int depth = 0; depth < pictureDepth; depth++) {

		for (int picturey = 0; picturey < pictureSize - 1; picturey += stride) { // pictureSize -1 or -2 depends on KernelSize

			for (int picturex = 0; picturex < pictureSize - 1; picturex += stride) {  // pictureSize -1 or -2 depends on KernelSize 


					//cout << "Filter Depth: " << depth << endl;
				for (int filtery = 0; filtery < kernelSize; filtery++) { // first moving filter down ( y-Direction ) [][Y]

					for (int filterx = 0; filterx < kernelSize; filterx++) {

						int xn = picturex + filterx; // coordinates picture where the filter "lays" on 
						int yn = picturey + filtery;

						if (inputData[xn][yn][depth] > tempMax)
							tempMax = inputData[xn][yn][depth];
							//cout << "Temp: " << tempMax << endl;
							//cout << "Filter X Koord. " << " Filter y Koord. " << " Pic. pos. x " << " Pic. pos. y " << " Depth "  << endl;
							//cout << "        " << filterx << "                " << filtery << "               " << xn << "        " << yn << "          " << depth  << endl;						
						}

					}
					//cout << "MAXIMUM: " << tempMax << endl;
					outputData[outputX][outputY][depth] = tempMax;
					tempMax = 0;
					//cout << "Output Data Position: " << outputX << " " << outputY << " " << depth << endl << endl;
					//cout << "Output Data: " << outputData[outputX][outputY][depth] << endl;
					outputX++;

					
			}outputX =0;
			outputY++;

		}outputY = 0;

	}

}

template<class Dtype>
inline std::array<unsigned, 3> pooling_layer<Dtype>::CalcShape( const std::array<unsigned, 3>& auDim, const layerparam_pooling<Dtype> & lp )
{
	return std::array<unsigned, 3>(
	{
		(unsigned) ( ( auDim[ 0 ] - lp.kernelSize ) / (Dtype) lp.stride + 1 ),
		(unsigned) ( ( auDim[ 1 ] - lp.kernelSize ) / (Dtype) lp.stride + 1 ),
		auDim[ 2 ]
	} );
}