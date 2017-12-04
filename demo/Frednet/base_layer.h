#ifndef BASE_LAYER_H
#define BASE_LAYER_H

struct layerParameter {

	std::string layerName = "";
	int numOutput = 0;
	int kernelSize = 0;
	int stride = 0;
	int padding = 0;
	int concatDim = 0;
	int pool = 0;
	array4D<float> convWeights;
	array2D<float> fcWeights;
	array1D<float> bias;
};

template <class type>
class base_layer
{
public:
	// specified constructor
	base_layer( array3D<type> &inputData, const outputShape &outputShape, std::string sLayerName );

	array3D<type> &getInput();
	array3D<type> &getOutput();

	// destructor
	virtual ~base_layer();

protected:
	array3D<type> &inputData;
	array3D<type> outputData;
	std::string m_sLayerName;
};

template <class type>
base_layer<type>::base_layer( array3D<type> &inputData, const outputShape &outputShape, std::string sLayerName ) :
	inputData( inputData ),
	outputData( { outputShape.outputWidth, outputShape.outputHeight, outputShape.outputDepth } ),
	m_sLayerName( sLayerName )
{

}

template <class type>
array3D<type> &base_layer<type>::getInput()
{
	return inputData;
}

template <class type>
array3D<type> &base_layer<type>::getOutput()
{
	return outputData;
}
template <class type>
base_layer<type>::~base_layer()
{
}



#endif
