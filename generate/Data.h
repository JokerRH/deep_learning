#pragma once

#include "Render\Vector.h"
#include "Queue.h"
#include <limits>
#include <pthread.h>
#include <fstream>
#include <regex>
#include <opencv2\core.hpp>
#include <opencv2\objdetect.hpp>

class CData
{
public:
	static bool Init( void );
	static bool OpenWrite( const std::wstring &sFile, unsigned uNumThreads = 1 );
	static void CloseWrite( void );

	CData( void ) = default;
	CData( const std::wstring &sFile, const std::wstring &sPath, bool fLoadImage = true );
	CData( const CData &other ) = default;
	CData( CData &&other ) = default;
	CData &operator=( const CData &other ) = default;
	CData &operator=( CData &&other ) = default;

	std::wstring ToString( unsigned int uPrecision = std::numeric_limits< double >::max_digits10 ) const;
	void WriteAsync( void );

	cv::Mat matImage;
	cv::Rect rectFace;
	cv::Point ptEyeLeft;
	cv::Point ptEyeRight;
	CVector<3> vec3EyeLeft;
	CVector<3> vec3EyeRight;
	CVector<3> vec3GazePoint;
	std::wstring sImage;

protected:
	bool LoadImage( const std::wstring &sImage, const std::string sWindow );

private:
	bool LoadImage( const std::wstring &sPath );
	static void *WriteThread( void * );
	bool GetEyePos( const cv::Mat &matFace, const std::string &sWindow );
	bool GetFaceRect( const std::string &sWindow );

	static const std::wregex s_regLine;
	static cv::CascadeClassifier s_FaceCascade;

	static std::wstring s_sPathWrite;
	static std::wfstream s_smFileWrite;
	static CQueue<CData> s_QueueWrite;
	static std::vector<pthread_t> s_vecThreadWrite;
};