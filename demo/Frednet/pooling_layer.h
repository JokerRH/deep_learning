#ifndef POOLING_LAYER_H
#define POOLING_LAYER_H


template <class type>
class pooling_layer :
	public base_layer<type>
{
public:
	struct layerparam
	{
		int kernelSize;
		int stride;
		int pool;
		std::string layerName = "";
	};

	pooling_layer( const layerparam &lp, array3D<type> &inputData );
	pooling_layer( const layerparam &lp, base_layer<type> &parentLayer );

	void forward();
	~pooling_layer() override;


private:
	int kernelSize;
	int stride;
	int pool;
};


// IMPLEMENTATION
template<class type>
inline pooling_layer<type>::pooling_layer( const layerparam &lp, array3D<type> &inputData ) :
	base_layer( inputData, outputShape( inputData.aiDim[ 0 ], inputData.aiDim[ 1 ], inputData.aiDim[ 2 ] ), lp.layerName ),
	kernelSize( lp.kernelSize ),
	stride( lp.stride ),
	pool( lp.pool )
{

}

template<class type>
inline pooling_layer<type>::pooling_layer( const layerparam &lp, base_layer<type> &parentLayer ) :
	pooling_layer( lp, parentLayer.getInput( ) )
{
}

// doing forward path (pooling)
//*******************************************************************************
// TODO: PictureSize has to be changed for unsymetric input!!!!!!!!!!
// DONE:	Check passing pictureDepth and Picture Size via parameter
//
//*******************************************************************************
template <class type>
void pooling_layer<type>::forward()
{

	// Parameters read from Layer parameter struct
	int stride = this->stride;
	int pictureDepth = inputData.aiDim[ 2 ];
	int pictureSize = inputData.aiDim[ 1 ];   // has to be changed for unsymetric input!!!!!!!!!!
	type tempMax = 0;
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





template <class type>
pooling_layer<type>::~pooling_layer()
{
}



#endif
