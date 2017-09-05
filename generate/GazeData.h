#include "Data.h"
#include "Render\Ray.h"
#include <regex>

class CGazeData : public CData
{
public:
	static bool CGazeData::Export( std::vector<CData> &vecData, const std::wstring &sPath, unsigned uValBatchSize, double dTrainValRatio );
	static std::vector<CGazeData> LoadData( const std::wstring &sPath );
	static cv::Rect FindTemplate( const cv::Mat &matImage, const cv::Mat &matTemplate );

	CGazeData( const CData &data );
	CGazeData( const std::string &sData, const std::string &sLabel );
	~CGazeData( void ) = default override;

	CData MergeReference( const std::vector<CData> &vecData );
	
	std::string ToString( unsigned uPrecision = std::numeric_limits<double>::max_digits10 ) const;
	
	CRay rayEyeLeft;
	CRay rayEyeRight;

private:
	static const std::regex s_regData;
	static const std::regex s_regLabel;
}