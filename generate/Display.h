#pragma once

#include <Windows.h>
#include <Windowsx.h>
#undef max
#undef min
#undef ERROR
#undef STRICT

#include "Data.h"
#include "Canon.h"
#include "Scenery.h"
#include <string>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif
#include <math.h>

class CDisplay
{
public:
	static cv::Rect Show( const std::string &sWindow, const cv::Mat &matImage );
	static void ShowImage( const std::string &sWindow, const CData &data, const CData &dataref = CData( ) );
	static void ShowLive( const std::string &sWindow, CCanon &camera );

private:
	CDisplay( double dRatio );
	CDisplay( const CData &data );

	void SetData( const cv::Mat &matImage, double dFOV );

	static cv::Size GetScreenResolution( void );
	void DrawFace( const CData &data );
	cv::Mat GetSceneryROI( void );
	void Show( const std::string &sWindow );

	static bool ProcessKey( CScenery &scenery, const CData *pBegin, const CData *pEnd, unsigned uKey, bool fVSC );
	static bool ProcessKey( CScenery &scenery, std::initializer_list<const CData *> listData, unsigned uKey, bool fVSC );
	bool ProcessMouse( CScenery &scenery, MSG &msg );

	std::string m_sReference = "Reference";
	int m_iFontFace = cv::FONT_HERSHEY_SIMPLEX;
	double m_dFontScale = 2;
	int m_iFontThickness = 3;

	//Calculate image size
	cv::Mat m_matScreen;
	cv::Rect m_rectTotal;
	cv::Rect m_rectScenery;
	cv::Rect m_rectImage;
	cv::Point m_ptText;

	cv::Point m_ptLastPos;
	bool m_fDrag = false;

	std::vector<CData> m_vecData;
	CScenery m_Scenery;
};

inline cv::Mat CDisplay::GetSceneryROI( void )
{
	return m_matScreen( m_rectScenery );
}

inline void CDisplay::Show( const std::string &sWindow )
{
	cv::imshow( sWindow, m_matScreen );
}

inline bool CDisplay::ProcessKey( CScenery &scenery, const CData *pBegin, const CData *pEnd, unsigned uKey, bool fVSC )
{
	if( fVSC )
		switch( uKey )
		{
		case 71:	//Pos1
			scenery *= CTransformation::GetRotationMatrixRad( 0, M_PI * 0.125, 0 );
			return true;
		case 72:	//Arrow_Up
			scenery *= CTransformation::GetTranslationMatrix( CVector<3>( { 0, 0.1, 0 } ) );
			return true;
		case 73:	//Img_Up
			scenery *= CTransformation::GetRotationMatrixRad( 0, 0, M_PI * 0.125 );
			return true;
		case 75:	//Arrow_Left
			scenery *= CTransformation::GetTranslationMatrix( CVector<3>( { -0.1, 0, 0 } ) );
			return true;
		case 77:	//Arrow_Right
			scenery *= CTransformation::GetTranslationMatrix( CVector<3>( { 0.1, 0, 0 } ) );
			return true;
		case 79:	//Home
			scenery *= CTransformation::GetRotationMatrixRad( 0, -M_PI * 0.125, 0 );
			return true;
		case 80:	//Arrow_Down
			scenery *= CTransformation::GetTranslationMatrix( CVector<3>( { 0, -0.1, 0 } ) );
			return true;
		case 81:	//Img_Down
			scenery *= CTransformation::GetRotationMatrixRad( 0, 0, -M_PI * 0.125 );
			return true;
		case 82:	//Insert
			scenery *= CTransformation::GetRotationMatrixRad( M_PI * 0.125, 0, 0 );
			return true;
		case 83:	//Delete
			scenery *= CTransformation::GetRotationMatrixRad( -M_PI * 0.125, 0, 0 );
			return true;
		default:
			return false;
		}
	else
		switch( uKey )
		{
		case 43:	//Numpad+
			scenery *= CTransformation::GetScaleMatrix( 1.1 );
			return true;
		case 45:	//Numpad-
			scenery *= CTransformation::GetScaleMatrix( 1 / 1.1 );
			return true;
		case 50:	//Numpad_2
			scenery = CScenery( pBegin, pEnd );
			scenery *= CTransformation::GetRotationMatrixRad( 0, M_PI, M_PI );
			return true;
		case 56:	//Numpad_8
			scenery = CScenery( pBegin, pEnd );
			scenery *= CTransformation::GetRotationMatrixRad( 0, M_PI, M_PI / 2 );
			return true;
		case 70:	//'f'
			scenery.Fit( false );
			return true;
		case 90:	//'z'
			scenery = CScenery( pBegin, pEnd );
			scenery.Fit( false );
			return true;
		default:
			return false;
		}
}

inline bool CDisplay::ProcessKey( CScenery &scenery, std::initializer_list<const CData *> listData, unsigned uKey, bool fVSC )
{
	return ProcessKey( scenery, (const CData *) listData.begin( ), (const CData *) listData.end( ), uKey, fVSC );
}

inline bool CDisplay::ProcessMouse( CScenery &scenery, MSG &msg )
{
	switch( msg.message )
	{
	case WM_LBUTTONDOWN:
		m_ptLastPos = cv::Point( GET_X_LPARAM( msg.lParam ), GET_Y_LPARAM( msg.lParam ) );
		if( !m_rectScenery.contains( m_ptLastPos ) )
			return false;

		m_fDrag = true;
		return true;
	case WM_LBUTTONUP:
		{
			if( !m_fDrag )
				return false;

			cv::Point pt( GET_X_LPARAM( msg.lParam ), GET_Y_LPARAM( msg.lParam ) );
			m_ptLastPos = pt - m_ptLastPos;
			scenery *= CTransformation::GetRotationMatrixRad( (double) m_ptLastPos.y / m_rectScenery.height * M_PI * 0.5, -(double) m_ptLastPos.x / m_rectScenery.width * M_PI * 0.5, 0 );
			m_fDrag = false;
			return true;
		}
	case WM_MOUSEMOVE:
		{
			if( !m_fDrag )
				return false;

			cv::Point pt( GET_X_LPARAM( msg.lParam ), GET_Y_LPARAM( msg.lParam ) );
			m_ptLastPos = pt - m_ptLastPos;
			scenery *= CTransformation::GetRotationMatrixRad( (double) m_ptLastPos.y / m_rectScenery.height * M_PI * 0.5, -(double) m_ptLastPos.x / m_rectScenery.width * M_PI * 0.5, 0 );
			m_ptLastPos = pt;
			return true;
		}
	default:
		return false;
	}
}