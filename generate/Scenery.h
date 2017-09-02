#pragma once

#include "Data.h"
#include "Render\RenderBox.h"
#include "Render\RenderPoint.h"
#include "Render\RenderLine.h"
#include "Render\Ray.h"
#include <opencv2\core.hpp>

class CScenery
{
public:
	CScenery( const CData &data );

	CScenery Transformed( const CMatrix<3, 3> &matTransform ) const;
	CScenery &Transform( const CMatrix<3, 3> &matTransform );
	CScenery &Fit( void );
	void Draw( cv::Mat &matImage ) const;

private:
	CRenderPoint m_Camera;
	CRay m_Forward;
	CRenderPoint m_GazePoint;
	CRenderBox m_Face;
	CRay m_GazeLeft;
	CRay m_GazeRight;
};