#ifdef WITH_FREDNET

#include "Detect.h"
#include "Compat.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>

#include "Frednet/caffe_parameter_parser.hpp"
#include "Frednet/base_layer.h"
#include "Frednet/conv_layer.h"
#include "Frednet/ReLu_layer.h"
#include "Frednet/pooling_layer.h"
#include "Frednet/mvn_layer.h"
#include "Frednet/flatten_layer.h"
#include "Frednet/fc_layer.h"

array3D<float> CDetect::s_Image( { IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_DEPTH } );
std::array<base_layer<float> *, 20> CDetect::s_aLayers = { nullptr };

bool CDetect::Init( const filestring_t &sPath )
{
	if( !CData::Init( sPath ) )
		return false;

	filestring_t sNetwork = compat::PathCombine_d( sPath, CFSTR( "network_fred" ) );
	if( !compat::PathFolderExists_d( sNetwork ) )
	{
		std::wcerr << "Network folder \"" << compat::ToWString( sNetwork ) << "\" does not exist" << std::endl;
		return false;
	}

	//conv1
	{
		layerparam_conv<float> lp( parseConvWeights2array<float>( "deploy.caffemodel_conv1_weights.txt" ), parseBias2array<float>( "deploy.caffemodel_conv1_bias.txt" ) );
		lp.layerName = "conv1";
		lp.stride = 4;
		lp.kernelSize = 11;
		lp.numOutput = 96;
		lp.padding = 0;
		s_aLayers[ 0 ] = new conv_layer<float>( lp, s_Image );
	}

	//relu1
	{
		layerparam_relu<float> lp;
		lp.layerName = "relu1";
		s_aLayers[ 1 ] = new relu_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 0 ] );
	}

	//pool1
	{
		layerparam_pooling<float> lp;
		lp.kernelSize = 3;
		lp.stride = 2;
		lp.pool = layerparam_pooling<float>::MAX;
		lp.layerName = "pool1";
		s_aLayers[ 2 ] = new pooling_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 1 ] );
	}

	//norm1
	{
		layerparam_mvn<float> lp;
		lp.layerName = "norm1";
		s_aLayers[ 3 ] = new mvn_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 2 ] );
	}

	//conv2
	{
		layerparam_conv<float> lp( parseConvWeights2array<float>( "deploy.caffemodel_conv2_weights.txt" ), parseBias2array<float>( "deploy.caffemodel_conv2_bias.txt" ) );
		lp.layerName = "conv2";
		lp.stride = 1;
		lp.kernelSize = 5;
		lp.numOutput = 256;
		lp.padding = 2;
		s_aLayers[ 4 ] = new conv_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 3 ] );
	}

	//relu2
	{
		layerparam_relu<float> lp;
		lp.layerName = "relu2";
		s_aLayers[ 5 ] = new relu_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 4 ] );
	}

	//pool2
	{
		layerparam_pooling<float> lp;
		lp.kernelSize = 3;
		lp.stride = 2;
		lp.pool = layerparam_pooling<float>::MAX;
		lp.layerName = "pool2";
		s_aLayers[ 6 ] = new pooling_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 5 ] );
	}

	//norm2
	{
		layerparam_mvn<float> lp;
		lp.layerName = "norm2";
		s_aLayers[ 7 ] = new mvn_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 6 ] );
	}

	//conv3
	{
		layerparam_conv<float> lp( parseConvWeights2array<float>( "deploy.caffemodel_conv3_weights.txt" ), parseBias2array<float>( "deploy.caffemodel_conv3_bias.txt" ) );
		lp.layerName = "conv3";
		lp.stride = 1;
		lp.kernelSize = 3;
		lp.numOutput = 384;
		lp.padding = 1;
		s_aLayers[ 8 ] = new conv_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 7 ] );
	}

	//relu3
	{
		layerparam_relu<float> lp;
		lp.layerName = "relu3";
		s_aLayers[ 9 ] = new relu_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 8 ] );
	}

	//conv4
	{
		layerparam_conv<float> lp( parseConvWeights2array<float>( "deploy.caffemodel_conv4_weights.txt" ), parseBias2array<float>( "deploy.caffemodel_conv4_bias.txt" ) );
		lp.layerName = "conv4";
		lp.stride = 1;
		lp.kernelSize = 3;
		lp.numOutput = 384;
		lp.padding = 1;
		s_aLayers[ 10 ] = new conv_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 9 ] );
	}

	//relu4
	{
		layerparam_relu<float> lp;
		lp.layerName = "relu4";
		s_aLayers[ 11 ] = new relu_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 10 ] );
	}

	//conv5
	{
		layerparam_conv<float> lp( parseConvWeights2array<float>( "deploy.caffemodel_conv5_weights.txt" ), parseBias2array<float>( "deploy.caffemodel_conv5_bias.txt" ) );
		lp.layerName = "conv5";
		lp.stride = 1;
		lp.kernelSize = 3;
		lp.numOutput = 256;
		lp.padding = 1;
		s_aLayers[ 12 ] = new conv_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 11 ] );
	}

	//relu5
	{
		layerparam_relu<float> lp;
		lp.layerName = "relu5";
		s_aLayers[ 13 ] = new relu_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 12 ] );
	}

	//pool5
	{
		layerparam_pooling<float> lp;
		lp.kernelSize = 3;
		lp.stride = 2;
		lp.pool = layerparam_pooling<float>::MAX;
		lp.layerName = "pool5";
		s_aLayers[ 14 ] = new pooling_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 13 ] );
	}

	//pool6
	{
		layerparam_pooling<float> lp;
		lp.kernelSize = 3;
		lp.stride = 2;
		lp.pool = layerparam_pooling<float>::AVE;
		lp.layerName = "pool6";
		s_aLayers[ 15 ] = new pooling_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 14 ] );
	}

	//relu6
	{
		layerparam_relu<float> lp;
		lp.layerName = "relu6";
		s_aLayers[ 16 ] = new relu_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 15 ] );
	}

	//pool7
	{
		layerparam_pooling<float> lp;
		lp.kernelSize = 3;
		lp.stride = 2;
		lp.pool = layerparam_pooling<float>::AVE;
		lp.layerName = "pool7";
		s_aLayers[ 17 ] = new pooling_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 16 ] );
	}

	//relu7
	{
		layerparam_relu<float> lp;
		lp.layerName = "relu6";
		s_aLayers[ 17 ] = new relu_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 16 ] );
	}

	//flatten8
	{
		layerparam_flatten<float> lp;
		lp.layerName = "flatten8";
		s_aLayers[ 18 ] = new flatten_layer<float>( lp, (image_layer<float> &) *s_aLayers[ 17 ] );
	}

	//fc8_gaze
	{
		layerparam_fc<float> lp( parseFcWeights2array<float>( "deploy.caffemodel_fc8_weights.txt" ), parseBias2array<float>( "deploy.caffemodel_fc8_bias.txt" ) );
		lp.layerName = "fc8";
		lp.numOutput = 8;
		s_aLayers[ 12 ] = new fc_layer<float>( lp, (base_flat_layer<float> &) *s_aLayers[ 11 ] );
	}

	return true;
}

void CDetect::Terminate( void )
{
	for( base_layer<float> *p : s_aLayers )
		delete p;
}

std::vector<cv::Rect> CDetect::GetFaces( const cv::Mat & matImage )
{
	cv::Mat matComp;
	cv::resize( matImage, matComp, cv::Size( 500, (int) ( 500.0 / matImage.cols * matImage.rows ) ) );
	std::vector<cv::Rect> vecFaces;
	s_FaceCascade.detectMultiScale( matComp, vecFaces, 1.1, 5, CV_HAAR_SCALE_IMAGE, cv::Size( 30, 30 ) );
	for( cv::Rect &rectFace : vecFaces )
	{
		rectFace = cv::Rect(
			(int) ( (double) rectFace.x / matComp.cols * matImage.cols ),
			(int) ( (double) rectFace.y / matComp.rows * matImage.rows ),
			(int) ( (double) rectFace.width / matComp.cols * matImage.cols ),
			(int) ( (double) rectFace.height / matComp.rows * matImage.rows ) );
	}

	return vecFaces;
}

CDetect::CDetect( const cv::Mat &matImage, const cv::Rect &rectFace, double dFOV, const filestring_t &sPath ) :
	CGazeData( matImage, rectFace, sPath )
{
	std::array<float, 8> arOutput = Forward( matImage( rectFace ) );
	/*
	std::wcout << "Output:" << std::endl;
	std::wcout << "\t" << arOutput[ 0 ] << std::endl;
	std::wcout << "\t" << arOutput[ 1 ] << std::endl;
	std::wcout << "\t" << arOutput[ 2 ] << std::endl;
	std::wcout << "\t" << arOutput[ 3 ] << std::endl;
	std::wcout << "\t" << arOutput[ 4 ] << std::endl;
	std::wcout << "\t" << arOutput[ 5 ] << std::endl;
	std::wcout << "\t" << arOutput[ 6 ] << std::endl;
	std::wcout << "\t" << arOutput[ 7 ] << std::endl;
	*/

	ptEyeLeft = cv::Point( (int) ( arOutput[ 4 ] * (double) rectFace.width ), (int) ( arOutput[ 5 ] * (double) rectFace.height ) );
	ptEyeRight = cv::Point( (int) ( arOutput[ 6 ] * (double) rectFace.width ), (int) ( arOutput[ 7 ] * (double) rectFace.height ) );

	CVector<2> vec2EyeLeft( { (double) ( rectFace.x + ptEyeLeft.x ), (double) ( rectFace.y + ptEyeLeft.y ) } );
	CVector<2> vec2EyeRight( { (double) ( rectFace.x + ptEyeRight.x ), (double) ( rectFace.y + ptEyeRight.y ) } );

	const double dIPDFrac = 0.066 / ( vec2EyeLeft - vec2EyeRight ).Abs( );
	const double dFocalLength = ( sqrt( matImage.cols * matImage.cols + matImage.rows * matImage.rows ) / 2 ) / tan( dFOV / ( 2 * 180 ) * M_PI );

	vec3EyeLeft = CVector<3>( {
		vec2EyeLeft[ 0 ] - matImage.cols / 2.0,
		-( vec2EyeLeft[ 1 ] - matImage.rows / 2.0 ),
		dFocalLength
	} ) * dIPDFrac;

	vec3EyeRight = CVector<3>( {
		vec2EyeRight[ 0 ] - matImage.cols / 2.0,
		-( vec2EyeRight[ 1 ] - matImage.rows / 2.0 ),
		dFocalLength
	} ) * dIPDFrac;

	rayEyeLeft = CRay( CVector<3>( { -0.5, 0, 0 } ), CVector<2>( { arOutput[ 0 ], arOutput[ 1 ] } ) );
	rayEyeRight = CRay( CVector<3>( { 0.5, 0, 0 } ), CVector<2>( { arOutput[ 2 ], arOutput[ 3 ] } ) );

#ifdef EXPORT_LOCAL
	//Rays are in local space of the face, transform to global space
	CTransformation matTransform( GetFaceTransformation( ) );
	rayEyeLeft = matTransform * rayEyeLeft;
	rayEyeRight = matTransform * rayEyeRight;
#else
	//Rays are in inverted global space
	CTransformation matTransform = CTransformation::GetRotationMatrix( CVector<3>( { 1, 0, 0 } ), CVector<3>( { 0, 1, 0 } ), CVector<3>( { 0, 0, -1 } ) );
	rayEyeLeft = matTransform * rayEyeLeft;
	rayEyeRight = matTransform * rayEyeRight;
	rayEyeLeft.m_vec3Origin = vec3EyeLeft;
	rayEyeRight.m_vec3Origin = vec3EyeRight;
#endif

	//Calculate gaze point as the point of shortest distance between the eye rays
	CVector<2> vec2Gaze = rayEyeLeft.PointOfShortestDistance( rayEyeRight );
	vec3GazePoint = ( rayEyeLeft( vec2Gaze[ 0 ] ) + rayEyeRight( vec2Gaze[ 1 ] ) ) / 2.0;
	rayEyeLeft *= vec2Gaze[ 0 ];
	rayEyeRight *= vec2Gaze[ 1 ];
}

std::array<float, 8> CDetect::Forward( cv::Mat matImage )
{
	cv::Size inputShape( IMAGE_WIDTH, IMAGE_HEIGHT );

	//Convert the input image to the input image format of the network
	if( matImage.channels( ) == 4 )
		cv::cvtColor( matImage, matImage, cv::COLOR_BGRA2BGR );
	else if( matImage.channels( ) == 1 )
		cv::cvtColor( matImage, matImage, cv::COLOR_GRAY2BGR );

	if( matImage.size( ) != inputShape )
		cv::resize( matImage, matImage, inputShape );

	//s_Image = ...

	for( base_layer<float> *pLayer : s_aLayers )
		pLayer->forward( );

	const float *pData = ( ( flat_layer<float> * ) s_aLayers.back( ) )->getOutput( ).data;
	return std::array<float, 8>{ pData[ 0 ], pData[ 1 ], pData[ 2 ], pData[ 3 ], pData[ 4 ], pData[ 5 ], pData[ 6 ], pData[ 7 ] };
}
#endif