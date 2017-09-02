#pragma once

#include "Data.h"
#include "Render\Vector.h"

class CCustom : public CData
{
public:
	struct fileformat
	{
		std::wstring sImage;
		double dFOV;
		CVector<3> vec3GazePoint;
	};

	static std::vector<fileformat> Load( const std::wstring &sPath, const std::wstring &sFile = std::wstring( ) );
	CCustom( const fileformat &data, const std::string &sWindow );
};