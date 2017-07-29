#pragma once

#include "LandmarkCandidate.h"
#include "BBox.h"
#include "Point.h"

class CLandmark
{
public:
	inline static std::vector<CLandmark> GetLandmarks( CImage &img, const char *szWindow )
	{
		return GetLandmarks( CLandmarkCandidate::GetCandidates( img ), szWindow );
	}

	static std::vector<CLandmark> GetLandmarks( std::vector<CLandmarkCandidate> vecCandidates, const char *szWindow );
	CLandmark( CLandmarkCandidate &candidate, const char *szWindow );
	void Draw( CImage &img );

	CBBox boxFace;
	CPoint ptEyeLeft;
	CPoint ptEyeRight;
	CPoint ptNose;
	double dDistance;

protected:
	CLandmark( void )
	{

	}
};