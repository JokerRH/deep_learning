/**
	\file
	\author Rainer Heinelt
*/
#pragma once

#include "Render\Vector.h"
#include "Render\Matrix.h"
#include "Render\Transformation.h"
#include "Queue.h"
#include <limits>
#include <pthread.h>
#include <fstream>
#include <regex>
#include <opencv2\core.hpp>
#include <opencv2\objdetect.hpp>

/**
	\brief Container holding gaze data

	Holds all information on a detected gaze
*/
class CData
{
public:
	static bool Init( const std::wstring &sPath );
	static bool OpenWrite( const std::wstring &sFile, unsigned uNumThreads = 1 );
	static void CloseWrite( void );
	static std::vector<CData> LoadData( const std::wstring &sFile, unsigned uCount = (unsigned) -1 );
	static void FindFilesRecursively( const std::wstring &sDir, const std::wstring &sPattern, std::vector<std::wstring> &vecsFiles );
	static std::wstring GetPath( const std::wstring &sFile );
	static cv::Rect ShowImage( const std::string &sWindow, const cv::Mat &matDraw );
	static std::wstring StrToWStr( const std::string &str );

	CData( void ) = default;
	CData( const std::wstring &sFile, const std::wstring &sPath, bool fLoadImage = true );
	CData( const CData &other ) = default;
	CData( CData &&other ) = default;
	CData &operator=( const CData &other ) = default;
	CData &operator=( CData &&other ) = default;
	virtual ~CData( void ) = default;

	bool LoadImage( void );
	std::wstring ToString( unsigned int uPrecision = std::numeric_limits< double >::max_digits10 ) const;
	void WriteAsync( void );
	void Show( const std::string &sWindow, const CData &dataref = CData( ) );
	void ScaleFace( const CVector<2> &vec2Scale, const CVector<2> &vec2Shift );
	bool IsValid( void ) const;

	CTransformation GetFaceTransformation( void ) const;

	cv::Mat matImage;			///< Image containing a face
	cv::Rect rectFace;			///< Bounding box for the face inside the image CData::matImage
	cv::Point ptEyeLeft;		///< Coordinate of the left eye relative to the bounding box CData::rectFace
	cv::Point ptEyeRight;		///< Coordinate of the right eye relative to the bounding box CData::rectFace
	CVector<3> vec3EyeLeft;		///< Coordinate of the left eye in 3 dimensional space (in meter)
	CVector<3> vec3EyeRight;	///< Coordinate of the right eye in 3 dimensional space (in meter)
	CVector<3> vec3GazePoint;	///< Coordinate of the gaze point in 3 dimensional space (in meter)
	std::wstring sImage;		///< Filename with extension of the loaded image
	std::wstring sRootPath;		///< Path to the image location
	bool fWriteImage = true;	///< Determines if the image should be written on save or not

protected:
	CData( const cv::Mat &matImage, const cv::Rect &rectFace, const std::wstring &sPath );
	bool LoadImage( const std::wstring &sImage, const std::string &sWindow );

	static cv::CascadeClassifier s_FaceCascade;	///<Haarcascade for face detection

private:
	static void *WriteThread( void * );
	bool GetEyePos( const cv::Mat &matFace, const std::string &sWindow );
	bool GetFaceRect( const std::string &sWindow );

	static const std::wregex s_regLine;				///< Regex used to extract saved information

	static std::wstring s_sPathWrite;				///< Path to whitch data is written on saving
	static std::wfstream s_smFileWrite;				///< File to whitch data is written on saving
	static CQueue<CData> s_QueueWrite;				///< Write queue
	static std::vector<pthread_t> s_vecThreadWrite;	///< Vector of write threads
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