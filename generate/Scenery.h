#pragma once

#include "Data.h"
#include "Render\RenderBox.h"
#include "Render\RenderPoint.h"
#include "Render\RenderLine.h"
#include "Render\Ray.h"
#include "Render\Transformation.h"
#include <opencv2\core.hpp>

class CScenery
{
public:
	CScenery( const CData &data );

	CScenery &Fit( bool fShift = true );
	void Draw( cv::Mat &matImage ) const;

	friend CScenery operator*( const CTransformation &matTransform, const CScenery &scenery );
	CScenery &operator*=( const CTransformation &matTransform );

	CTransformation GetTransformation( void ) const;

private:
	CRenderPoint m_Camera;
	CRay m_Forward;
	CRay m_Up;
	CRay m_Right;
	CRenderPoint m_GazePoint;
	CRenderBox m_Face;
	CRay m_GazeLeft;
	CRay m_GazeRight;

	CTransformation m_matTransform = CTransformation::Unit( );
};

CScenery operator*( const CTransformation &matTransform, const CScenery &scenery );

inline CTransformation CScenery::GetTransformation( void ) const
{
	return m_matTransform;
}