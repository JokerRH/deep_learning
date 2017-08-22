#pragma once

#include "LandmarkCandidate.h"
#include "BBox.h"
#include "Point.h"
#include "Render/Vector.h"

class CLandmark
{
public:
	CLandmark( const CLandmark &other );
	CLandmark &operator=( const CLandmark &other ) = default;

	CLandmark( CLandmark &&other ) = default;
	CLandmark &operator=( CLandmark &&other );

	static CBBox GetEyeBox( const std::deque<CBBox> &vecEyes, const CVector<2> &vec2Pos );
	static CPoint GetPoint( CBBox &box );
	static CPoint GetPointManual( CBBox &box, CPoint pt, const char *szWindow );
	void Adjust( const char *szWindow );
	inline static std::vector<CLandmark> GetLandmarks( CImage &img )
	{
		return GetLandmarks( CLandmarkCandidate::GetCandidates( img ) );
	}

	static std::vector<CLandmark> GetLandmarks( std::vector<CLandmarkCandidate> vecCandidates );
	CLandmark( CLandmarkCandidate &candidate );
	CLandmark( const CBBox &boxFace, const CPoint &ptEyeLeft, const CPoint &ptEyeRight );
	void Draw( CImage &img );

	CBBox boxFace;
	CPoint ptEyeLeft;
	CPoint ptEyeRight;

protected:
	CLandmark( void )
	{

	}
};

inline CLandmark::CLandmark( const CLandmark &other ) :
	boxFace( other.boxFace ),
	ptEyeLeft( other.ptEyeLeft ),
	ptEyeRight( other.ptEyeRight )
{
	ptEyeLeft.TransferOwnership( boxFace );
	ptEyeRight.TransferOwnership( boxFace );
}

inline CLandmark &CLandmark::operator=( CLandmark &&other )
{
	this->~CLandmark( );
	new( this ) CLandmark( std::move( other ) );
	return *this;
}