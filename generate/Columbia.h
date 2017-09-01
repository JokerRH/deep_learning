#pragma once

#include "Data.h"
#include <vector>

class CColumbiaData : public CData
{
public:
	static std::vector<std::wstring> Load( const std::wstring &sPath, const std::wstring &sFile = std::wstring( ) );
	CColumbiaData( const std::wstring &sPath, const std::string &sWindow );
};