#include "RenderPoint.h"
#include <cstdlib> // NULL 

CRenderPoint* CRenderPoint::ms_instance = NULL;

CRenderPoint::CRenderPoint()
{
}

CRenderPoint::~CRenderPoint()
{
}

CRenderPoint* CRenderPoint::Instance()
{
	if (ms_instance == NULL) {
		ms_instance = new CRenderPoint();
	}
	return ms_instance;
}

void CRenderPoint::Release()
{
	if (ms_instance) {
		delete ms_instance;
	}
	ms_instance = NULL;
}

