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

class CData
{
public:
	static bool Init( void );
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

	cv::Mat matImage;
	cv::Rect rectFace;
	cv::Point ptEyeLeft;
	cv::Point ptEyeRight;
	CVector<3> vec3EyeLeft;
	CVector<3> vec3EyeRight;
	CVector<3> vec3GazePoint;
	std::wstring sImage;
	std::wstring sRootPath;
	bool fWriteImage = true;

protected:
	CData( const cv::Mat &matImage, const cv::Rect &rectFace, const std::wstring &sPath );
	bool LoadImage( const std::wstring &sImage, const std::string &sWindow );

	static cv::CascadeClassifier s_FaceCascade;

private:
	static void *WriteThread( void * );
	bool GetEyePos( const cv::Mat &matFace, const std::string &sWindow );
	bool GetFaceRect( const std::string &sWindow );

	static const std::wregex s_regLine;

	static std::wstring s_sPathWrite;
	static std::wfstream s_smFileWrite;
	static CQueue<CData> s_QueueWrite;
	static std::vector<pthread_t> s_vecThreadWrite;
};

inline bool CData::IsValid( void ) const
{
	return !matImage.empty( );
}