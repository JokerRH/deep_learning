#pragma once

#include <assert.h>
#include <vector>
#include "Render/Vector.h"

class CImage;
class CBaseBBox;
class CBaseHighlighter
{
public:
	~CBaseHighlighter( void );
	virtual unsigned int GetPositionX( unsigned int uLevel = -1 ) const;
	virtual unsigned int GetPositionY( unsigned int uLevel = -1 ) const;
	virtual double GetRelPositionX( unsigned int uLevel = -1 ) const;
	virtual double GetRelPositionY( unsigned int uLevel = -1 ) const;
	virtual CImage *GetImage( unsigned int uLevel = -1 );
	virtual CBaseBBox *GetParent( unsigned int uLevel = -1 );
	void Shift( const CVector<2> &vec2Offset );
	bool IsValid( void ) const;

	virtual void TransferOwnership( unsigned int uLevel = 1 );
	virtual void TransferOwnership( CBaseBBox &parentBox );

	const char szName[ 32 ];

protected:
	CBaseHighlighter( void );
	CBaseHighlighter( const char *szName );
	CBaseHighlighter( CBaseBBox &parentBox, unsigned int uX, unsigned int uY, unsigned int uLevel, const char *szName );
	CBaseHighlighter( CBaseBBox &parentBox, double dX, double dY, const char *szName );
	CBaseHighlighter( CBaseBBox &parentBox, const CVector<2> &vec2Pos, const char *szName );
	CBaseHighlighter( const CBaseHighlighter &other );

	void Swap( CBaseHighlighter &other, bool fSwapChildren = true );

	inline void AddChild( CBaseHighlighter *pChild )
	{
		m_vecpChildren.push_back( pChild );
	}

	bool RemoveChild( CBaseHighlighter *pChild );

	CBaseBBox *m_pParentBox;
	std::vector<CBaseHighlighter *> m_vecpChildren;

	double m_dPositionX = 0;	//Left X-position in % of parent's width
	double m_dPositionY = 0;	//Top Y-position in % of parent's height

	friend CImage;
};

inline bool CBaseHighlighter::IsValid( void ) const
{
	return ( strcmp( szName, "Unassigned" ) );
}