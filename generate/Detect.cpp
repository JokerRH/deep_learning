#include "Detect.h"
#include <Windows.h>
#include <Pathcch.h>
#include <opencv2\imgproc.hpp>
#include <opencv2\objdetect.hpp>

caffe::Net<float> *CDetect::s_pNetwork;

bool CDetect::Init( const std::wstring &sNetwork )
{
	caffe::Caffe::set_mode( caffe::Caffe::CPU );

	//Get proto and model file path
	std::string sProtoFile;
	std::string sModelFile;
	{
		WCHAR szFullPattern[ MAX_PATH ];
		PathCchCombine( szFullPattern, MAX_PATH, sNetwork.c_str( ), L"deploy.prototxt" );
		sProtoFile = std::string( szFullPattern, szFullPattern + wcslen( szFullPattern ) );

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
			if( modelFileData.dwFileAttributes & ~( FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY ) )
				continue;

			goto FOUND;
		} while( FindNextFile( hModelFile, &modelFileData ) );

		FindClose( hModelFile );
		std::wcout << "No caffemodel found in folder \"" << sNetwork << "\"" << std::endl;
		return false;

FOUND:
		PathCchCombine( szFullPattern, MAX_PATH, sNetwork.c_str( ), modelFileData.cFileName );
		sModelFile = std::string( szFullPattern, szFullPattern + wcslen( szFullPattern ) );
	}

	//Load network
	s_pNetwork = new caffe::Net<float>( sProtoFile, caffe::TEST );
	return false;
}

CDetect::CDetect( const cv::Mat &matImage, const cv::Rect &rectFace, double dFOV ) :
	CData( matImage, rectFace )
{

}