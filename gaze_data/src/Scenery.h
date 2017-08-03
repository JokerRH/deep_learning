#pragma once

#include "Render/Vector.h"
#include "Render/Matrix.h"
#include "Image.h"
#include "Ray.h"
#include "Render/Render.h"

class CScenery
{
public:
	static void SetScenery( const CVector<3> &vec3MonitorPos, const CVector<3> vec3MonitorDim );
	CScenery( const CRay &rayEyeLeft, const CRay &rayEyeRight );
	CScenery( const CVector<3> &vec3MonitorPos, const CVector<3> &vec3MonitorDim, const CRay &rayEyeLeft, const CRay &rayEyeRight );
	CScenery Transformed( const CMatrix<3, 3> &matTransform ) const;
	CScenery &Transform( const CMatrix<3, 3> &matTransform );
	CScenery &Fit( void );
	void Draw( CImage &img ) const;

private:
	static CVector<3> s_vec3MonitorPos;
	static CVector<3> s_vec3MonitorDim;
	
	CRenderBox m_Monitor;
	CRenderBox m_Face;
	CRenderPoint m_Camera;

	CVector<3> m_avec3Monitor[ 8 ];
	CVector<3> m_vec3Camera;
	CVector<3> m_avec3Face[ 8 ];
	CRay m_rayEyeLeft;
	CRay m_rayEyeRight;
};

