#pragma once

#include "image_layer.h"
#include <array>
#include <cmath>

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
	unsigned padding;
	mode pool;
	std::string layerName = std::string( "" );
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
	unsigned padding;
	int pool;
};


// IMPLEMENTATION
template<class Dtype>
inline pooling_layer<Dtype>::pooling_layer( const layerparam_pooling<Dtype> &lp, const array3D<Dtype> &inputData ) :
	image_layer<Dtype>( inputData, CalcShape( inputData.auDim, lp ), lp.layerName ),
	kernelSize( lp.kernelSize ),
	stride( lp.stride ),
	padding( lp.padding ),
	pool( lp.pool )
{
	assert( !padding );	//Padding not yet implemented!
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
	std::cout << "\"" << image_layer<Dtype>::m_sLayerName << "\" running" << std::endl;
	unsigned uWidth = image_layer<Dtype>::inputData.auDim[ 0 ];
	unsigned uHeight = image_layer<Dtype>::inputData.auDim[ 1 ];

	if( pool == layerparam_pooling<Dtype>::MAX )
		for( unsigned uZ = 0; uZ < image_layer<Dtype>::inputData.auDim[ 2 ]; uZ++ )
			for( unsigned uY = 0, uOY = 0; uY < uHeight; uY += stride, uOY++ )
				for( unsigned uX = 0, uOX = 0; uX < uWidth; uX += stride, uOX++ )
				{
					Dtype rMax = image_layer<Dtype>::inputData[ uX ][ uY ][ uZ ];
					for( unsigned uFilterY = uY; uFilterY < std::min( uY + kernelSize, uHeight ); uFilterY++ )
						for( unsigned uFilterX = uX; uFilterX < std::min( uX + kernelSize, uWidth ); uFilterX++ )
							rMax = std::max( rMax, image_layer<Dtype>::inputData[ uFilterX ][ uFilterY ][ uZ ] );

					image_layer<Dtype>::outputData[ uOX ][ uOY ][ uZ ] = rMax;
				}
	else if( pool == layerparam_pooling<Dtype>::AVE )
		for( unsigned uZ = 0; uZ < image_layer<Dtype>::inputData.auDim[ 2 ]; uZ++ )
			for( unsigned uY = 0, uOY = 0; uY < uHeight; uY += stride, uOY++ )
				for( unsigned uX = 0, uOX = 0; uX < uWidth; uX += stride, uOX++ )
				{
					Dtype rAve = 0;
					for( unsigned uFilterY = uY; uFilterY < std::min( uY + kernelSize, uHeight ); uFilterY++ )
						for( unsigned uFilterX = uX; uFilterX < std::min( uX + kernelSize, uWidth ); uFilterX++ )
							rAve += image_layer<Dtype>::inputData[ uFilterX ][ uFilterY ][ uZ ];

					image_layer<Dtype>::outputData[ uOX ][ uOY ][ uZ ] = rAve / ( kernelSize * kernelSize ) ;
				}
	else
		assert( false );	//Not yet implemented

#if 0	//Caffe source
	for (int n = 0; n < bottom[0]->num(); ++n) {
		for (int c = 0; c < channels_; ++c) {
			for (int ph = 0; ph < pooled_height_; ++ph) {
				for (int pw = 0; pw < pooled_width_; ++pw) {
					int hstart = ph * stride_h_ - pad_h_;
					int wstart = pw * stride_w_ - pad_w_;
					int hend = min(hstart + kernel_h_, height_);
					int wend = min(wstart + kernel_w_, width_);
					hstart = max(hstart, 0);
					wstart = max(wstart, 0);
					const int pool_index = ph * pooled_width_ + pw;
					for (int h = hstart; h < hend; ++h) {
						for (int w = wstart; w < wend; ++w) {
							const int index = h * width_ + w;
								if (bottom_data[index] > top_data[pool_index]) {
									top_data[pool_index] = bottom_data[index];
									if (use_top_mask) {
										top_mask[pool_index] = static_cast<Dtype>(index);
									} else {
										mask[pool_index] = index;
									}
								}
							}
						}
					}
				}

				xxxxxx
				aaa
				  aaa
				    aa

				// compute offset
				bottom_data += bottom[0]->offset(0, 1);
				top_data += top[0]->offset(0, 1);
				if (use_top_mask) {
					top_mask += top[0]->offset(0, 1);
				} else {
					mask += top[0]->offset(0, 1);
				}
			}
		}
#endif
}

template<class Dtype>
inline std::array<unsigned, 3> pooling_layer<Dtype>::CalcShape( const std::array<unsigned, 3>& auDim, const layerparam_pooling<Dtype> & lp )
{
	assert( auDim[ 0 ] + 2 * lp.padding >= lp.kernelSize && auDim[ 1 ] + 2 * lp.padding >= lp.kernelSize );
	return std::array<unsigned, 3>(
	{
		static_cast<unsigned>( std::ceil( static_cast<Dtype>( auDim[ 0 ] + 2 * lp.padding - lp.kernelSize ) / lp.stride ) ) + 1,
		static_cast<unsigned>( std::ceil( static_cast<Dtype>( auDim[ 1 ] + 2 * lp.padding - lp.kernelSize ) / lp.stride ) ) + 1,
		auDim[ 2 ]
	} );

	/* Caffe source:
	pooled_height_ = static_cast<int>(ceil(static_cast<float>(
		height_ + 2 * pad_h_ - kernel_h_) / stride_h_)) + 1;
	pooled_width_ = static_cast<int>(ceil(static_cast<float>(
		width_ + 2 * pad_w_ - kernel_w_) / stride_w_)) + 1;
	*/
}
