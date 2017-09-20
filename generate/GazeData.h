#pragma once

#include "Data.h"
#include "Render\Ray.h"
#include <regex>

class CGazeData : public CData
{
public:
	static bool CGazeData::Export( std::vector<CData> &vecData, std::wstring sPath, unsigned uValBatchSize, double dTrainValRatio = 2.0 / 3.0 );
	static std::vector<CGazeData> LoadData( const std::wstring &sPath );
	static cv::Rect FindTemplate( const cv::Mat &matImage, const cv::Mat &matTemplate );

	CGazeData( const CData &data );
	CGazeData( const std::string &sData, const std::string &sLabel, const std::wstring &sPath );
	~CGazeData( void ) override = default;

	CData MergeReference( const std::vector<CData> &vecData );
	void PrintDiff( const CData &dataref ) const;
	
	std::string ToString( unsigned uPrecision = std::numeric_limits<double>::max_digits10 ) const;
	
	CRay rayEyeLeft;
	CRay rayEyeRight;

protected:
	CGazeData( const cv::Mat &matImage, const cv::Rect &rectFace, const std::wstring &sPath );

private:
	static const std::regex s_regData;
	static const std::regex s_regLabel;
};

inline CGazeData::CGazeData( const cv::Mat &matImage, const cv::Rect &rectFace, const std::wstring &sPath ) :
	CData( matImage, rectFace, sPath )
{

}