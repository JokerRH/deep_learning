/**
\file
\author Rainer Heinelt
*/
#pragma once

#include "Compat.h"
#include "Render/Vector.h"
#include "Render/Matrix.h"
#include "Render/Transformation.h"
#include "Queue.h"
#include <limits>
#include <fstream>
#include <regex>
#include <thread>
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>

/**
\brief Container holding gaze data

Holds all information on a detected gaze
*/
class CData
{
public:
	static bool Init( const filestring_t &sPath );

	CData( void ) = default;
	CData( const CData &other ) = default;
	CData( CData &&other ) = default;
	CData &operator=( const CData &other ) = default;
	CData &operator=( CData &&other ) = default;
	virtual ~CData( void ) = default;

	bool IsValid( void ) const;

	CTransformation GetFaceTransformation( void ) const;

	cv::Mat matImage;			///< Image containing a face
	cv::Rect rectFace;			///< Bounding box for the face inside the image CData::matImage
	cv::Point ptEyeLeft;		///< Coordinate of the left eye relative to the bounding box CData::rectFace
	cv::Point ptEyeRight;		///< Coordinate of the right eye relative to the bounding box CData::rectFace
	CVector<3> vec3EyeLeft;		///< Coordinate of the left eye in 3 dimensional space (in meter)
	CVector<3> vec3EyeRight;	///< Coordinate of the right eye in 3 dimensional space (in meter)
	CVector<3> vec3GazePoint;	///< Coordinate of the gaze point in 3 dimensional space (in meter)
	filestring_t sImage;		///< Filename with extension of the loaded image
	filestring_t sRootPath;		///< Path to the image location
	bool fWriteImage = true;	///< Determines if the image should be written on save or not

protected:
	CData( const cv::Mat &matImage, const cv::Rect &rectFace, const filestring_t &sPath );

	static cv::CascadeClassifier s_FaceCascade;	///<Haarcascade for face detection

private:
	bool GetEyePos( const cv::Mat &matFace, const std::string &sWindow );
	bool GetFaceRect( const std::string &sWindow );
};

/**
\brief Checks wether the instance is valid or not
\retval true The instance is valid
\retval false The instance is invalid

If the image CData::matImage is empty the instance is considered invalid.
*/
inline bool CData::IsValid( void ) const
{
	return !matImage.empty( );
}