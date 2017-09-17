#ifdef WITH_CAFFE

#include "Detect.h"
#include <Windows.h>
#include <Shlwapi.h>
#include <Pathcch.h>
#include <opencv2\imgproc.hpp>
#include <opencv2\objdetect.hpp>

#pragma warning( push )
#pragma warning( disable: 4244 )
#pragma warning( disable: 4996 )
#define _SCL_SECURE_NO_WARNINGS
#include <caffe\layer_factory.hpp>
#pragma warning( pop )

#pragma comment( lib, "libcaffe.lib" )

caffe::Net<float> *CDetect::s_pNetwork;
cv::Size CDetect::s_InputShape;
cv::Mat CDetect::s_matMean;

using namespace caffe;

namespace caffe
{
	extern void Register_InputLayer( void );
	extern void Register_ConvolutionLayer( void );
	extern void Register_PoolingLayer( void );
	extern void Register_DSPPLayer( void );
	extern void Register_LRNLayer( void );
	extern void Register_ReLULayer( void );
	extern void Register_SplitLayer( void );
	extern void Register_InnerProductLayer( void );
	extern void Register_DropoutLayer( void );
	extern void Register_MemoryDataLayer( void );
}

bool CDetect::Init( const std::wstring &sNetwork )
{
	if( !PathFileExists( sNetwork.c_str( ) ) )
	{
		std::wcerr << "Network folder \"" << sNetwork << "\" does not exist" << std::endl;
		return false;
	}

	Register_InputLayer( );
	Register_ConvolutionLayer( );
	Register_PoolingLayer( );
	Register_DSPPLayer( );
	Register_LRNLayer( );
	Register_ReLULayer( );
	Register_SplitLayer( );
	Register_InnerProductLayer( );
	Register_DropoutLayer( );
	Register_MemoryDataLayer( );

	caffe::Caffe::set_mode( caffe::Caffe::CPU );

	//Get proto and model file path
	std::string sProtoFile;
	std::string sMeanFile;
	std::string sModelFile;
	{
		WCHAR szFullPattern[ MAX_PATH ];
		PathCchCombine( szFullPattern, MAX_PATH, sNetwork.c_str( ), L"deploy.prototxt" );
		sProtoFile = std::string( szFullPattern, szFullPattern + wcslen( szFullPattern ) );
		if( !PathFileExists( szFullPattern ) || GetFileAttributes( szFullPattern ) & FILE_ATTRIBUTE_DIRECTORY )
		{
			std::wcerr << "Protofile \"" << szFullPattern << "\" not found" << std::endl;
			return false;
		}

		PathCchCombine( szFullPattern, MAX_PATH, sNetwork.c_str( ), L"data.binaryproto" );
		sMeanFile = std::string( szFullPattern, szFullPattern + wcslen( szFullPattern ) );
		if( !PathFileExists( szFullPattern ) || GetFileAttributes( szFullPattern ) & FILE_ATTRIBUTE_DIRECTORY )
		{
			std::wcerr << "Mean file \"" << szFullPattern << "\" not found" << std::endl;
			return false;
		}

		PathCchCombine( szFullPattern, MAX_PATH, sNetwork.c_str( ), L"*.caffemodel" );
		WIN32_FIND_DATA modelFileData;
		HANDLE hModelFile = FindFirstFile( szFullPattern, &modelFileData );
		if( hModelFile == INVALID_HANDLE_VALUE )
		{
			FindClose( hModelFile );
			std::wcout << "No caffemodel found in folder \"" << sNetwork << "\"" << std::endl;
			return false;
		}

		do
		{
			if( modelFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				continue;

			goto FOUND;
		} while( FindNextFile( hModelFile, &modelFileData ) );

		FindClose( hModelFile );
		std::wcout << "No caffemodel file found in folder \"" << sNetwork << "\"" << std::endl;
		return false;

FOUND:
		PathCchCombine( szFullPattern, MAX_PATH, sNetwork.c_str( ), modelFileData.cFileName );
		sModelFile = std::string( szFullPattern, szFullPattern + wcslen( szFullPattern ) );
	}

	//Load network
	s_pNetwork = new caffe::Net<float>( sProtoFile, caffe::TEST );
	s_pNetwork->CopyTrainedLayersFrom( sModelFile );
	std::wcout << "Done loading" << std::endl;

	CHECK_EQ( s_pNetwork->num_inputs( ), 1 ) << "Network should have exactly one input.";
	CHECK_EQ( s_pNetwork->num_outputs( ), 1 ) << "Network should have exactly one output.";

	Blob<float> *pInputLayer = s_pNetwork->input_blobs( )[ 0 ];
	CHECK_EQ( pInputLayer->channels( ), 3 ) << "Input layer should have 3 channels";
	Blob<float> *pOutputLayer = s_pNetwork->output_blobs( )[ 0 ];
	CHECK_EQ( pOutputLayer->channels( ), 8 ) << "Output layer should have 3 channels";

	s_InputShape = cv::Size( pInputLayer->width( ), pInputLayer->height( ) );
	if( !SetMean( sMeanFile ) )
	{
		std::wcerr << "Failed to set mean file \"" << StrToWStr( sMeanFile ) << "\"" << std::endl;
		delete s_pNetwork;
		return false;
	}

	return true;
}

void CDetect::Terminate( void )
{
	delete s_pNetwork;
}

CDetect::CDetect( const cv::Mat &matImage, const cv::Rect &rectFace, double dFOV ) :
	CData( matImage, rectFace )
{

}

bool CDetect::SetMean( const std::string &sMeanFile )
{
	BlobProto blob_proto;
	if( !ReadProtoFromBinaryFile( sMeanFile.c_str( ), &blob_proto ) )
		return false;

	//Convert from BlobProto to Blob<float>
	Blob<float> mean_blob;
	mean_blob.FromProto( blob_proto );

	CHECK_EQ( mean_blob.channels( ), 3 ) << "Number of channels of mean file doesn't match input layer.";

	//The format of the mean file is planar 32-bit float BGR
	std::vector<cv::Mat> channels;
	float *data = mean_blob.mutable_cpu_data( );
	for( int i = 0; i < 3; i++ )
	{
		//Extract an individual channel
		cv::Mat channel( mean_blob.height( ), mean_blob.width( ), CV_32FC1, data );
		channels.push_back( channel );

		data += mean_blob.height( ) * mean_blob.width( );
	}

	//Merge the separate channels into a single image
	cv::Mat mean;
	cv::merge( channels, mean );

	//Compute the global mean pixel value and create a mean image filled with this value
	cv::Scalar channel_mean = cv::mean( mean );

	s_matMean = cv::Mat( s_InputShape, mean.type( ), channel_mean );
	return true;
}

std::array<float, 8> CDetect::Forward( cv::Mat matImage )
{
	Blob<float> *pInputLayer = s_pNetwork->input_blobs( )[ 0 ];
	pInputLayer->Reshape( 1, 3, s_InputShape.height, s_InputShape.width );
	s_pNetwork->Reshape( );	//Forward dimension change to all layers

	//Separate channels
	std::vector<cv::Mat> vecInputChannels;
	{
		float* prInputData = pInputLayer->mutable_cpu_data( );
		for( int i = 0; i < pInputLayer->channels( ); i++ )
		{
			vecInputChannels.emplace_back( s_InputShape.height, s_InputShape.width, CV_32FC1, prInputData );
			prInputData += s_InputShape.width * s_InputShape.height;
		}
	}

	//Convert the input image to the input image format of the network
	if( matImage.channels( ) == 4 )
		cv::cvtColor( matImage, matImage, cv::COLOR_BGRA2BGR );
	else if( matImage.channels( ) == 1 )
		cv::cvtColor( matImage, matImage, cv::COLOR_GRAY2BGR );

	if( matImage.size( ) != s_InputShape )
		cv::resize( matImage, matImage, s_InputShape );
	
	matImage.convertTo( matImage, CV_32FC3 );
	cv::subtract( matImage, s_matMean, matImage );



	//This operation will write the separate BGR planes directly to the input layer of the network because it is wrapped by the cv::Mat objects in vecInputChannels
	cv::split( matImage, vecInputChannels );
	CHECK( reinterpret_cast<float*>( vecInputChannels.at( 0 ).data ) == pInputLayer->cpu_data( ) ) << "Input channels are not wrapping the input layer of the network.";

	s_pNetwork->Forward( );

	//Copy the output layer to a std::vector
	Blob<float>* pOutputLayer = s_pNetwork->output_blobs( )[ 0 ];
	const float* prBegin = pOutputLayer->cpu_data( );
	std::array<float, 8> arData;
	std::copy( prBegin, prBegin + 8, arData.begin( ) );
	return arData;
}
#endif