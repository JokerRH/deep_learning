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
	static std::vector<CData> LoadData( const std::wstring &sFile, unsigned uCount = (unsigned) -1 );
	static void FindFilesRecursively( const std::wstring &sDir, const std::wstring &sPattern, std::vector<std::wstring> &vecsFiles );
	static std::wstring GetPath( const std::wstring &sFile );
	static cv::Rect ShowImage( const std::string &sWindow, const cv::Mat &matDraw );
	static bool Export( std::vector<CData> &vecData, const std::wstring &sPath, unsigned uValBatchSize, double dTrainValRatio = 2.0 / 3.0 );
	static std::vector<CData> Import( const std::wstring &sPath );
	static std::wstring StrToWStr( const std::string &str );
	static cv::Rect FindTemplate( const cv::Mat &matImage, const cv::Mat &matTemplate );

	CData( void ) = default;
	CData( const std::wstring &sFile, const std::wstring &sPath, bool fLoadImage = true );
	CData( const CData &other ) = default;
	CData( CData &&other ) = default;
	CData &operator=( const CData &other ) = default;
	CData &operator=( CData &&other ) = default;

	bool LoadImage( void );
	CData ImportLoad( const std::vector<CData> &vecData );
	std::wstring ToString( unsigned int uPrecision = std::numeric_limits< double >::max_digits10 ) const;
	std::string ToCSV( unsigned int uPrecision = std::numeric_limits< double >::max_digits10 ) const;
	void WriteAsync( void );
	void Show( const std::string &sWindow );
	void ScaleFace( const CVector<2> &vec2Scale, const CVector<2> &vec2Shift );
	bool IsValid( void );

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
	bool LoadImage( const std::wstring &sImage, const std::string &sWindow );

private:
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

inline bool CData::IsValid( void )
{
	return !matImage.empty( );
}