#pragma once

#include "Data.h"
#include "Render/RenderObject.h"
#include "Render/RenderBox.h"
#include "Render/RenderPoint.h"
#include "Render/RenderLine.h"
#include "Render/Ray.h"
#include "Render/Transformation.h"
#include <opencv2/core/core.hpp>

class CSceneryFace : public CRenderObject
{
public:
	enum RenderStage
	{
		STAGE_1,
		STAGE_2,
		STAGE_3
	};

	CSceneryFace( const CData &data );
	~CSceneryFace( void ) override = default;

	CVector<3> GetMin( void ) const override;
	CVector<3> GetMax( void ) const override;
	std::wstring ToString( unsigned int uPrecision = 2 ) const override;

	friend CSceneryFace operator*( const CTransformation &matTransform, const CSceneryFace &scenery );
	CSceneryFace &operator*=( const CTransformation &matTransform );
	void Draw( cv::Mat &matImage, RenderStage stage ) const;

	CRenderPoint m_GazePoint;
	CRenderBox m_Face;
	CRay m_GazeLeft;
	CRay m_GazeRight;
};

class CScenery
{
public:
	CScenery( const CData &data );
	CScenery( const std::initializer_list<const CData *> &list );
	CScenery( const CData *pBegin, const CData *pEnd );

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
	std::vector<CSceneryFace> m_vecFaces;
};

inline CVector<3> CSceneryFace::GetMin( void ) const
{
	return CRenderObject::GetMin( { &m_GazePoint, &m_Face } );
}

inline CVector<3> CSceneryFace::GetMax( void ) const
{
	return CRenderObject::GetMax( { &m_GazePoint, &m_Face } );
}

inline std::wstring CSceneryFace::ToString( unsigned int uPrecision ) const
{
	return m_Face.ToString( uPrecision ) + L"; " + m_GazePoint.ToString( uPrecision );
}

CSceneryFace operator*( const CTransformation &matTransform, const CSceneryFace &face );
CScenery operator*( const CTransformation &matTransform, const CScenery &scenery );