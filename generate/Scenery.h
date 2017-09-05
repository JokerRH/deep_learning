#pragma once

#include "Data.h"
#include "Render\RenderBox.h"
#include "Render\RenderPoint.h"
#include "Render\RenderLine.h"
#include "Render\Ray.h"
#include "Render\RenderHelper.h"
#include <opencv2\core.hpp>

class CScenery
{
public:
	CScenery( const CData &data );

	CScenery &Transform( const CMatrix<3, 3> &matTransform );
	CScenery Transformed( const CMatrix<3, 3> &matTransform ) const;
	CScenery &Shift( const CVector<3> &vec3Shift );
	CScenery Shifted( const CVector<3> &vec3Shift ) const;
	CScenery &Fit( bool fShift = true );
	void Draw( cv::Mat &matImage ) const;
	CMatrix<3, 3> GetTransformation( void );
	CVector<3, 3> GetShift( void );

private:
	CRenderPoint m_Camera;
	CRay m_Forward;
	CRay m_Up;
	CRay m_Right;
	CRenderPoint m_GazePoint;
	CRenderBox m_Face;
	CRay m_GazeLeft;
	CRay m_GazeRight;
};

inline CScenery CScenery::Transformed( const CMatrix<3, 3> &matTransform ) const
{
	return CScenery( *this ).Transform( matTransform );
}

inline CScenery CScenery::Shifted( const CVector<3> &vec3Shift ) const
{
	return CScenery( *this ).Shift( vec3Shift );
}