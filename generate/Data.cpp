/**
\file
\author Rainer Heinelt
\brief CData implementation
*/

#include "Data.h"
#include "Scenery.h"
#include "Render\Transformation.h"
#include "Render\Ray.h"
#include <iostream>
#include <wtypes.h>
#include <Windows.h>
#include <Windowsx.h>
#include <Shlwapi.h>
#include <sstream>
#include <regex>
#include <Pathcch.h>
#include <malloc.h>
#include <algorithm>
#include <random>
#include <string>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>

#undef LoadImage
#undef max
#undef min

const std::wregex CData::s_regLine(LR"a((?:"((?:[^"]|"")*)"\s+|(\S+)\s+)\(\s*([+]?\d+)\s*,\s*([+]?\d+)\)[xX]\(\s*([+]?\d+)\s*,\s*([+]?\d+)\)\s+\(\s*([+]?\d+)\s*,\s*([+]?\d+)\)@\(\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\)\s+\(\s*([+]?\d+)\s*,\s*([+]?\d+)\)@\(\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\)\s+->\(\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\s*,\s*([+-]?(?:(?:\d+(?:\.\d+)?)|(?:\.\d+)))\).*)a");
cv::CascadeClassifier CData::s_FaceCascade;
std::wstring CData::s_sPathWrite;
std::wfstream CData::s_smFileWrite;
CQueue<CData> CData::s_QueueWrite(10);
std::vector<std::thread> CData::s_vecThreadWrite;

/**
\fn static bool CData::Init( const std::wstring &sPath )
\brief Initializes this class
\param[in]	sPath	Path to the OpenCV Haarcascade folder
\retval		true	Class sucessfully initialized
\retval		false	Error initializing (failed to load Haarcascade)
\asdf

Loads the OpenCV Haarcascade for face detection (\see CData::s_FaceCascade)
*/
bool CData::Init(const std::wstring &sPath)
{
	WCHAR szPath[MAX_PATH];
	PathCchCombine(szPath, MAX_PATH, sPath.c_str(), L"haarcascade_frontalface_default.xml");
	if (!s_FaceCascade.load(std::string(szPath, szPath + wcslen(szPath))))
	{
		std::wcerr << "Unable to load face cascade" << std::endl;
		return false;
	}

	return true;
}

/**
\fn
\brief Opens the file for writing
\param[in]	sFile		File to be written to
\param[in]	uNumThreads	Number of write threads (\see CData::s_vecThreadWrite)
\retval		true		Sucessfully initialized writer
\retval		false		Failed to initialized writer

If it doesn't exist, creates the directory the file should be stored in, then opens the file for writing.
Write threads (\see CData::s_vecThreadWrite) are created.
*/
bool CData::OpenWrite(const std::wstring &sFile, unsigned uNumThreads)
{
	s_sPathWrite = GetPath(sFile);

	if (!CreateDirectory(s_sPathWrite.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		std::wcerr << "Unable to create directory \"" << s_sPathWrite << "\"" << std::endl;
		return false;
	}

	s_smFileWrite.open(sFile, std::wfstream::app);
	if (!s_smFileWrite.is_open())
	{
		std::wcerr << "Unable to open file \"" << sFile << "\" for writing" << std::endl;
		return false;
	}

	for (unsigned u = 0; u < uNumThreads; u++)
		s_vecThreadWrite.emplace_back(WriteThread);

	return true;
}

void CData::CloseWrite(void)
{
	for (unsigned u = 0; u < s_vecThreadWrite.size(); u++)
		s_QueueWrite.Emplace_Back();

	for (std::thread &thread : s_vecThreadWrite)
		thread.join();

	s_smFileWrite.close();
}

std::vector<CData> CData::LoadData(const std::wstring &sFile, unsigned uCount)
{
	std::vector<CData> vecData;
	std::wfstream smFile(sFile, std::wfstream::in);
	if (!smFile.is_open())
	{
		std::wcerr << "Unable to open file \"" << sFile << "\" for reading" << std::endl;
		return vecData;
	}

	std::wstring sLine;
	while (std::getline(smFile, sLine))
	{
		if (!uCount--)
			break;

		try
		{
			vecData.emplace_back(sLine, GetPath(sFile), false);
		}
		catch (int)
		{

		}
	}

	smFile.close();
	return vecData;
}

void CData::FindFilesRecursively(const std::wstring &sDir, const std::wstring &sPattern, std::vector<std::wstring> &vecsFiles)
{
	const WCHAR *lpFolder = sDir.c_str();
	const WCHAR *lpFilePattern = sPattern.c_str();
	WCHAR szFullPattern[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile;

	//Process subdirectories
	PathCchCombine(szFullPattern, MAX_PATH, lpFolder, L"*");
	hFindFile = FindFirstFile(szFullPattern, &FindFileData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && FindFileData.cFileName[0] != '.')
			{
				//Found subdirectory, process
				PathCchCombine(szFullPattern, MAX_PATH, lpFolder, FindFileData.cFileName);
				FindFilesRecursively(szFullPattern, lpFilePattern, vecsFiles);
			}
		} while (FindNextFile(hFindFile, &FindFileData));
		FindClose(hFindFile);
	}

	// Now we are going to look for the matching files
	PathCchCombine(szFullPattern, MAX_PATH, lpFolder, lpFilePattern);
	hFindFile = FindFirstFile(szFullPattern, &FindFileData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				//Found file
				PathCchCombine(szFullPattern, MAX_PATH, lpFolder, FindFileData.cFileName);
				vecsFiles.push_back(szFullPattern);
			}
		} while (FindNextFile(hFindFile, &FindFileData));
		FindClose(hFindFile);
	}
}

std::wstring CData::GetPath(const std::wstring &sFile)
{
	WCHAR *szPath = (WCHAR*)_malloca((sFile.size() + 1) * sizeof(WCHAR));
	if (!szPath)
	{
		std::wcerr << "Unable to allcate memory for path" << std::endl;
		throw 0;
	}
	memcpy((void *)szPath, sFile.c_str(), (sFile.size() + 1) * sizeof(WCHAR));
	PathCchRemoveFileSpec(szPath, (sFile.size() + 1) * sizeof(WCHAR));
	return std::wstring(szPath);
}

static cv::Size GetScreenResolution(void)
{
	RECT desktop;
	GetWindowRect(GetDesktopWindow(), &desktop);
	return cv::Size(desktop.right, desktop.bottom);
}

cv::Rect CData::ShowImage(const std::string &sWindow, const cv::Mat &matImage)
{
	cv::Mat mat(GetScreenResolution(), CV_8UC3, cv::Scalar::all(255));

	double dScale = mat.cols / (double)matImage.cols;
	if (matImage.rows * dScale > (double)mat.rows)
		dScale = mat.rows / (double)matImage.rows;

	cv::Size size((int)(matImage.cols * dScale), (int)(matImage.rows * dScale));
	cv::Rect rect((mat.cols - size.width) / 2, (mat.rows - size.height) / 2, size.width, size.height);
	cv::resize(matImage, mat(rect), size);

	cv::imshow(sWindow, mat);
	return rect;
}

std::wstring CData::StrToWStr(const std::string &str)
{
	const std::ctype<wchar_t> &ctype = std::use_facet<std::ctype<wchar_t>>(std::locale());
	std::vector<wchar_t> wideStringBuffer(str.length());
	ctype.widen(str.data(), str.data() + str.length(), &wideStringBuffer[0]);
	return std::wstring(&wideStringBuffer[0], wideStringBuffer.size());
}

CData::CData(const std::wstring &sLine, const std::wstring &sPath, bool fLoadImage) :
	sRootPath(sPath)
{
	std::wsmatch match;
	std::regex_match(sLine, match, s_regLine);
	if (!match.size())
	{
		std::wcerr << "Unable to parse line \"" << sLine << "\"" << std::endl;
		throw 0;
	}

	if (match[1].matched)
		sImage = std::regex_replace(match[1].str(), std::wregex(L"\"\""), L"\"");
	else
		sImage = match[2].str();

	rectFace = cv::Rect(std::stoi(match[3].str()), std::stoi(match[4].str()), std::stoi(match[5].str()), std::stoi(match[6].str()));
	ptEyeLeft = cv::Point(std::stoi(match[7].str()), std::stoi(match[8].str()));
	vec3EyeLeft = CVector<3>({ std::stod(match[9].str()), std::stod(match[10].str()), std::stod(match[11].str()) });
	ptEyeRight = cv::Point(std::stoi(match[12].str()), std::stoi(match[13].str()));
	vec3EyeRight = CVector<3>({ std::stod(match[14].str()), std::stod(match[15].str()), std::stod(match[16].str()) });
	vec3GazePoint = CVector<3>({ std::stod(match[17].str()), std::stod(match[18].str()), std::stod(match[19].str()) });

	if (!fLoadImage)
		return;

	//Load image
	LoadImage();
}

/**
\brief Loads the image from disk
\retval	true	Sucessfully loaded the image
\retval	false	Failed to load the image

Loads the image by combining CData::sRootPath and CData::sImage.
*/
bool CData::LoadImage(void)
{
	WCHAR szFullPattern[MAX_PATH];
	PathCchCombine(szFullPattern, MAX_PATH, sRootPath.c_str(), sImage.c_str());
	std::wstring sImage(szFullPattern);

	matImage = cv::imread(std::string(sImage.begin(), sImage.end()));
	if (matImage.empty())
	{
		std::wcerr << "Unable to read image \"" << sImage << "\"" << std::endl;
		return false;
	}

	return true;
}

std::wstring CData::ToString(unsigned int uPrecision) const
{
	std::wostringstream out;
	out.setf(std::ios_base::fixed, std::ios_base::floatfield);
	out.precision(uPrecision);

	out << "\"" << std::regex_replace(sImage, std::wregex(L"\""), L"\"\"") << "\"";
	out << " (" << rectFace.x << ", " << rectFace.y << ")X(" << rectFace.width << ", " << rectFace.height << ")";
	out << " (" << ptEyeLeft.x << ", " << ptEyeLeft.y << ")@(" << vec3EyeLeft[0] << ", " << vec3EyeLeft[1] << ", " << vec3EyeLeft[2] << ")";
	out << " (" << ptEyeRight.x << ", " << ptEyeRight.y << ")@(" << vec3EyeRight[0] << ", " << vec3EyeRight[1] << ", " << vec3EyeRight[2] << ")";
	out << " ->(" << vec3GazePoint[0] << ", " << vec3GazePoint[1] << ", " << vec3GazePoint[2] << ")";
	return out.str();
}

void CData::WriteAsync(void)
{
	s_QueueWrite.Push_Back(*this);
}

static void DrawFace(const CData &data, const cv::Rect &rectImage, cv::Mat &matScreen)
{
	cv::Mat matImage = data.matImage.clone();
	double dScaleX = (double)rectImage.width / data.matImage.cols;
	double dScaleY = (double)rectImage.height / data.matImage.rows;
	cv::resize(matImage, matImage, rectImage.size());
	cv::Rect rectFace((int)(data.rectFace.x * dScaleX), (int)(data.rectFace.y * dScaleY), (int)(data.rectFace.width * dScaleX), (int)(data.rectFace.height * dScaleY));
	cv::Point ptEyeLeft((int)(data.ptEyeLeft.x * dScaleX), (int)(data.ptEyeLeft.y * dScaleY));
	cv::Point ptEyeRight((int)(data.ptEyeRight.x * dScaleX), (int)(data.ptEyeRight.y * dScaleY));
	cv::rectangle(matImage, rectFace, cv::Scalar(0, 0, 255), 2);
	cv::circle(matImage, ptEyeLeft + rectFace.tl(), 1, cv::Scalar(0, 255, 0), -1);
	cv::circle(matImage, ptEyeRight + rectFace.tl(), 1, cv::Scalar(255, 0, 0), -1);
	matImage.copyTo(matScreen(rectImage));
}

void CData::Show(const std::string &sWindow, const CData &dataref)
{
	//Reference text
	std::string sReference("Reference");
	int iFontFace = cv::FONT_HERSHEY_SIMPLEX;
	double dFontScale = 2;
	int iFontThickness = 3;

	//Calculate image size
	cv::Mat matScreen(GetScreenResolution(), CV_8UC3, cv::Scalar::all(255));
	cv::Rect rectTotal;
	cv::Rect rectScenery;
	cv::Rect rectImage;
	cv::Point ptText;
	{
		int iBaseLine = 0;
		cv::Size textSize(cv::getTextSize(sReference, iFontFace, dFontScale, iFontThickness, &iBaseLine));
		int iSideWidth = std::max(matImage.cols, textSize.width);

		cv::Size size(matImage.cols * 2 + iSideWidth, std::max(matImage.cols * 2, matImage.rows + textSize.height));
		double dScale = std::min((double)matScreen.cols / size.width, (double)matScreen.rows / size.height);
		size = cv::Size((int)(size.width * dScale), (int)(size.height * dScale));
		rectTotal = cv::Rect((int)(matScreen.cols / 2.0 - size.width / 2.0), (int)(matScreen.rows / 2.0 - size.height / 2.0), size.width, size.height);

		double dBaseDim = (double)matImage.cols / (matImage.cols * 2 + iSideWidth);
		rectScenery = cv::Rect(rectTotal.x, rectTotal.y, (int)(rectTotal.width * dBaseDim * 2), (int)(rectTotal.width * dBaseDim * 2));
		rectImage = cv::Rect(rectTotal.x + rectScenery.width, rectTotal.y, (int)(rectTotal.width * dBaseDim), (int)(rectTotal.width * dBaseDim / matImage.cols * matImage.rows));
		ptText = cv::Point(rectImage.x, rectImage.y + rectImage.height + textSize.height);
	}

	//Draw face
	DrawFace(*this, rectImage, matScreen);

	CScenery scenery(*this);
	scenery.Fit().Draw(matScreen(rectScenery));
	cv::imshow(sWindow, matScreen);
	bool fReference = false;

	bool fDrag = false;
	cv::Point ptLastPos;

	MSG msg;
	BOOL fReturn;
	while (true)
	{
		if ((fReturn = GetMessage(&msg, NULL, 0, 0)) == -1)
			throw 27;	//Error

		switch (msg.message)
		{
		case WM_QUIT:
			std::wcout << "Quit message" << std::endl;
			PostQuitMessage(0);
			throw 27;
		case WM_KEYDOWN:
		{
			unsigned uKey = MapVirtualKey((UINT)msg.wParam, MAPVK_VK_TO_CHAR);
			switch (uKey)
			{
			case 0:
				uKey = MapVirtualKey((UINT)msg.wParam, MAPVK_VK_TO_VSC);
				switch (uKey)
				{
				case 71:	//Pos1
					scenery *= CTransformation::GetRotationMatrixRad(0, M_PI * 0.125, 0);
					scenery.Draw(matScreen(rectScenery));
					cv::imshow(sWindow, matScreen);
					break;
				case 72:	//Arrow_Up
					scenery *= CTransformation::GetTranslationMatrix(CVector<3>({ 0, 0.1, 0 }));
					scenery.Draw(matScreen(rectScenery));
					cv::imshow(sWindow, matScreen);
					break;
				case 73:	//Img_Up
					scenery *= CTransformation::GetRotationMatrixRad(0, 0, M_PI * 0.125);
					scenery.Draw(matScreen(rectScenery));
					cv::imshow(sWindow, matScreen);
					break;
				case 75:	//Arrow_Left
					scenery *= CTransformation::GetTranslationMatrix(CVector<3>({ -0.1, 0, 0 }));
					scenery.Draw(matScreen(rectScenery));
					cv::imshow(sWindow, matScreen);
					break;
				case 77:	//Arrow_Right
					scenery *= CTransformation::GetTranslationMatrix(CVector<3>({ 0.1, 0, 0 }));
					scenery.Draw(matScreen(rectScenery));
					cv::imshow(sWindow, matScreen);
					break;
				case 79:	//Home
					scenery *= CTransformation::GetRotationMatrixRad(0, -M_PI * 0.125, 0);
					scenery.Draw(matScreen(rectScenery));
					cv::imshow(sWindow, matScreen);
					break;
				case 80:	//Arrow_Down
					scenery *= CTransformation::GetTranslationMatrix(CVector<3>({ 0, -0.1, 0 }));
					scenery.Draw(matScreen(rectScenery));
					cv::imshow(sWindow, matScreen);
					break;
				case 81:	//Img_Down
					scenery *= CTransformation::GetRotationMatrixRad(0, 0, -M_PI * 0.125);
					scenery.Draw(matScreen(rectScenery));
					cv::imshow(sWindow, matScreen);
					break;
				case 82:	//Insert
					scenery *= CTransformation::GetRotationMatrixRad(M_PI * 0.125, 0, 0);
					scenery.Draw(matScreen(rectScenery));
					cv::imshow(sWindow, matScreen);
					break;
				case 83:	//Delete
					scenery *= CTransformation::GetRotationMatrixRad(-M_PI * 0.125, 0, 0);
					scenery.Draw(matScreen(rectScenery));
					cv::imshow(sWindow, matScreen);
					break;
				default:
					std::wcout << "VSC Key: " << uKey << std::endl;
				}
				break;
			case 9:		//Tab
				if (!dataref.IsValid())
					break;

				if (fReference)
				{
					matScreen = cv::Scalar::all(255);
					scenery = scenery.GetTransformation() * CScenery(*this);
					DrawFace(*this, rectImage, matScreen);
				}
				else
				{
					scenery = scenery.GetTransformation() * CScenery(dataref);
					DrawFace(dataref, rectImage, matScreen);
					putText(matScreen, sReference, ptText, iFontFace, dFontScale, cv::Scalar(51, 153, 255), iFontThickness, 8);
				}

				fReference = !fReference;
				scenery.Draw(matScreen(rectScenery));
				cv::imshow(sWindow, matScreen);
				break;
			case 13:	//Enter
				return;
			case 27:
				throw 27;
			case 43:	//Numpad+
				scenery *= CTransformation::GetScaleMatrix(1.1);
				scenery.Draw(matScreen(rectScenery));
				cv::imshow(sWindow, matScreen);
				break;
			case 45:	//Numpad-
				scenery *= CTransformation::GetScaleMatrix(1 / 1.1);
				scenery.Draw(matScreen(rectScenery));
				cv::imshow(sWindow, matScreen);
				break;
			case 50:	//Numpad_2
				scenery = CScenery(*this);
				scenery *= CTransformation::GetRotationMatrixRad(0, M_PI, M_PI);
				scenery.Draw(matScreen(rectScenery));
				cv::imshow(sWindow, matScreen);
				break;
			case 56:	//Numpad_8
				scenery = CScenery(*this);
				scenery *= CTransformation::GetRotationMatrixRad(0, M_PI, M_PI / 2);
				scenery.Draw(matScreen(rectScenery));
				cv::imshow(sWindow, matScreen);
				break;
			case 70:	//'f'
				scenery.Fit(false).Draw(matScreen(rectScenery));
				cv::imshow(sWindow, matScreen);
				break;
			case 90:	//'z'
				scenery = CScenery(*this);
				DrawFace(*this, rectImage, matScreen);
				scenery.Fit(false).Draw(matScreen(rectScenery));
				cv::imshow(sWindow, matScreen);
				break;
			default:
				std::wcout << "Key: " << uKey << std::endl;
			}
			break;
		}
		case WM_LBUTTONDOWN:
			ptLastPos = cv::Point(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			if (!rectScenery.contains(ptLastPos))
				break;

			fDrag = true;
			break;
		case WM_LBUTTONUP:
		{
			if (!fDrag)
				break;

			cv::Point pt(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			ptLastPos = pt - ptLastPos;
			scenery *= CTransformation::GetRotationMatrixRad((double)ptLastPos.y / rectScenery.height * M_PI * 0.5, -(double)ptLastPos.x / rectScenery.width * M_PI * 0.5, 0);
			scenery.Draw(matScreen(rectScenery));
			cv::imshow(sWindow, matScreen);
			fDrag = false;
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (!fDrag)
				break;

			cv::Point pt(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
			ptLastPos = pt - ptLastPos;
			scenery *= CTransformation::GetRotationMatrixRad((double)ptLastPos.y / rectScenery.height * M_PI * 0.5, -(double)ptLastPos.x / rectScenery.width * M_PI * 0.5, 0);
			scenery.Draw(matScreen(rectScenery));
			cv::imshow(sWindow, matScreen);
			ptLastPos = pt;
			break;
		}
		}

		if (fReturn > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void CData::ScaleFace(const CVector<2> &vec2Scale, const CVector<2> &vec2Shift)
{
	cv::Point ptTL((int)(rectFace.x + matImage.cols * vec2Shift[0]), (int)(rectFace.y + matImage.rows * vec2Shift[1]));
	cv::Point ptBR((int)(rectFace.x + rectFace.width * vec2Scale[0]), (int)(rectFace.y + rectFace.height * vec2Scale[1]));

	ptEyeLeft += rectFace.tl();
	ptEyeRight += rectFace.tl();
	int iBuffer = (int)(rectFace.width * 0.1);
	ptTL = cv::Point(std::max(std::min({ ptTL.x, ptEyeLeft.x - iBuffer, ptEyeRight.x - iBuffer }), 0), std::max(std::min({ ptTL.y, ptEyeLeft.y - iBuffer, ptEyeRight.y - iBuffer }), 0));
	ptBR = cv::Point(std::min(std::max({ ptBR.x, ptEyeLeft.x + iBuffer, ptEyeRight.x + iBuffer }), matImage.cols), std::min(std::max({ ptBR.y, ptEyeLeft.y + iBuffer, ptEyeRight.y + iBuffer }), matImage.rows));
	rectFace = cv::Rect(ptTL, ptBR);
	ptEyeLeft -= rectFace.tl();
	ptEyeRight -= rectFace.tl();
}

CTransformation CData::GetFaceTransformation(void) const
{
	CVector<3> vec3EyesX = vec3EyeRight - vec3EyeLeft;
	double dScale = vec3EyesX.Abs();
	vec3EyesX /= dScale;
	CVector<3> vec3EyesY = vec3EyesX.CrossProduct(CVector<3>({ 0, 0, -1 })).Normalize();
	CVector<3> vec3EyesZ = vec3EyesY.CrossProduct(vec3EyesX).Normalize();

	return CTransformation::GetTRSMatrix((vec3EyeLeft + vec3EyeRight) / 2.0, vec3EyesX, vec3EyesY, vec3EyesZ, dScale);
}

void CData::WriteThread(void)
{
	WCHAR szFullPattern[MAX_PATH];
	while (true)
	{
		CData data = s_QueueWrite.Pop_Front();
		if (!data.IsValid())
			break;

		if (data.fWriteImage)
		{
			PathCchCombine(szFullPattern, MAX_PATH, s_sPathWrite.c_str(), data.sImage.c_str());
			if (!cv::imwrite(std::string(szFullPattern, szFullPattern + wcslen(szFullPattern)), data.matImage))
			{
				std::wcerr << L"Failed to write image to \"" << szFullPattern << L"\"" << std::endl;
				continue;
			}
		}

		s_smFileWrite << data.ToString() << std::endl;
	}
}

bool CData::GetEyePos(const cv::Mat &matFace, const std::string &sWindow)
{
	ptEyeLeft = cv::Point(0, 0);
	ptEyeRight = cv::Point(0, 0);
	cv::Mat matDraw = matFace.clone();
	cv::circle(matDraw, ptEyeLeft, 4, cv::Scalar(0, 255, 0), -1);
	cv::circle(matDraw, ptEyeRight, 4, cv::Scalar(255, 0, 0), -1);
	cv::Rect rect = ShowImage(sWindow, matDraw);

	MSG msg;
	BOOL fReturn;
	while (true)
	{
		if ((fReturn = GetMessage(&msg, NULL, 0, 0)) == -1)
			throw 27;	//Error

		switch (msg.message)
		{
		case WM_QUIT:
			PostQuitMessage(0);
			throw 27;
		case WM_KEYDOWN:
		{
			unsigned uKey = MapVirtualKey((UINT)msg.wParam, MAPVK_VK_TO_CHAR);
			switch (uKey)
			{
			case 8:		//Backspace
				throw 0;
			case 13:	//Enter
				return true;
			case 27:
				throw 27;
			case 69:	//'e'
				return false;
			default:
				std::wcout << "Key: " << uKey << std::endl;
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			int iX = GET_X_LPARAM(msg.lParam);
			int iY = GET_Y_LPARAM(msg.lParam);
			iX -= rect.x;
			iY -= rect.y;
			if (iX < 0 || iY < 0 || iX > rect.width || iY > rect.height)
				break;

			cv::Point pt((int)((double)iX / rect.width * rectFace.width), (int)((double)iY / rect.height * rectFace.height));
			if ((double)iX / rect.width > 0.5)
				ptEyeLeft = pt;
			else
				ptEyeRight = pt;

			matDraw = matFace.clone();
			cv::circle(matDraw, ptEyeLeft, 4, cv::Scalar(0, 255, 0), -1);
			cv::circle(matDraw, ptEyeRight, 4, cv::Scalar(255, 0, 0), -1);
			rect = ShowImage(sWindow, matDraw);
			break;
		}
		case WM_RBUTTONDOWN:
			return true;
		}

		if (fReturn > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

bool CData::GetFaceRect(const std::string &sWindow)
{
	cv::Mat matImage;
	cv::resize(this->matImage, matImage, cv::Size(720, (int)(720.0 / this->matImage.cols * this->matImage.rows))); //Resize image to improve performance

	cv::Mat matDraw = matImage.clone();
	cv::rectangle(matDraw, rectFace, cv::Scalar(0, 0, 255), 3);
	cv::Rect rect = ShowImage(sWindow, matDraw);
	bool fOrigin = true;

	MSG msg;
	BOOL fReturn;
	while (true)
	{
		if ((fReturn = GetMessage(&msg, NULL, 0, 0)) == -1)
			throw 27;	//Error

		switch (msg.message)
		{
		case WM_QUIT:
			PostQuitMessage(0);
			throw 27;
		case WM_KEYDOWN:
		{
			unsigned uKey = MapVirtualKey((UINT)msg.wParam, MAPVK_VK_TO_CHAR);
			switch (uKey)
			{
			case 8:		//Backspace
				throw 0;
			case 13:	//Enter
				return true;
			case 27:
				throw 27;
			default:
				std::wcout << "Key: " << uKey << std::endl;
			}
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (fOrigin)
				break;

			int iX = GET_X_LPARAM(msg.lParam);
			int iY = GET_Y_LPARAM(msg.lParam);
			iX -= rect.x;
			iY -= rect.y;
			if (iX < 0 || iY < 0 || iX > rect.width || iY > rect.height)
				break;

			cv::Point ptClick((int)((double)iX / rect.width * matImage.cols), (int)((double)iY / rect.height * matImage.rows));
			cv::Rect rectFace = cv::Rect(this->rectFace.tl(), ptClick);	//Do not override class face while moving

			matDraw = matImage.clone();
			cv::rectangle(matDraw, rectFace, cv::Scalar(0, 255, 255), 1);
			rect = ShowImage(sWindow, matDraw);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			int iX = GET_X_LPARAM(msg.lParam);
			int iY = GET_Y_LPARAM(msg.lParam);
			iX -= rect.x;
			iY -= rect.y;
			if (iX < 0 || iY < 0 || iX > rect.width || iY > rect.height)
				break;

			cv::Point ptClick((int)((double)iX / rect.width * matImage.cols), (int)((double)iY / rect.height * matImage.rows));
			if (fOrigin)
				rectFace = cv::Rect(ptClick, ptClick);
			else
				rectFace = cv::Rect(rectFace.tl(), ptClick);

			fOrigin = !fOrigin;

			matDraw = matImage.clone();
			cv::rectangle(matDraw, rectFace, cv::Scalar(0, 0, 255), 1);
			rect = ShowImage(sWindow, matDraw);
			break;
		}
		case WM_RBUTTONDOWN:
			if (!fOrigin)
				break;

			rectFace = cv::Rect(
				(int)((double)rectFace.x / matImage.cols * this->matImage.cols),
				(int)((double)rectFace.y / matImage.rows * this->matImage.rows),
				(int)((double)rectFace.width / matImage.cols * this->matImage.cols),
				(int)((double)rectFace.height / matImage.rows * this->matImage.rows));

			return true;
		}

		if (fReturn > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

CData::CData(const cv::Mat &matImage, const cv::Rect &rectFace, const std::wstring &sPath) :
	matImage(matImage.clone()),
	rectFace(rectFace)
{
	if (sPath.empty())
		return;

	sImage = std::wstring(PathFindFileName(sPath.c_str()));
}

bool CData::LoadImage(const std::wstring &sImage, const std::string &sWindow)
{
	this->sImage = std::wstring(PathFindFileName(sImage.c_str()));
	matImage = cv::imread(std::string(sImage.begin(), sImage.end()));
	if (matImage.empty())
	{
		std::wcerr << "Unable to read image \"" << sImage << "\"" << std::endl;
		return false;
	}

	//Detect face
	{
		cv::Mat matComp;
		cv::resize(matImage, matComp, cv::Size(500, (int)(500.0 / matImage.cols * matImage.rows)));
		std::vector<cv::Rect> vecFaces;
		s_FaceCascade.detectMultiScale(matComp, vecFaces, 1.1, 5, CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
		if (!vecFaces.size())
			GetFaceRect(sWindow);
		else
			rectFace = cv::Rect(
			(int)((double)vecFaces.front().x / matComp.cols * matImage.cols),
				(int)((double)vecFaces.front().y / matComp.rows * matImage.rows),
				(int)((double)vecFaces.front().width / matComp.cols * matImage.cols),
				(int)((double)vecFaces.front().height / matComp.rows * matImage.rows));
	}

	while (!GetEyePos(matImage(rectFace), sWindow))
		GetFaceRect(sWindow);

	return true;
}