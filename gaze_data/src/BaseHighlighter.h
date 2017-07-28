#pragma once

#include <assert.h>
#include <vector>

class CImage;
class CBaseBBox;
class CBaseHighlighter
{
public:
	~CBaseHighlighter( void );
	virtual unsigned int GetPositionX( unsigned int uLevel = -1 ) const;
	virtual unsigned int GetPositionY( unsigned int uLevel = -1 ) const;
	virtual CImage *GetImage( unsigned int uLevel = -1 );
	virtual CBaseBBox *GetParent( unsigned int uLevel = -1 );

	virtual void TransferOwnership( unsigned int uLevel = 1 );
	virtual void TransferOwnership( CBaseBBox &parentBox );

	const char szName[ 32 ];

protected:
	CBaseHighlighter( void );
	CBaseHighlighter( const char *szName );
	CBaseHighlighter( CBaseBBox &parentBox, unsigned int uX, unsigned int uY, unsigned int uLevel, const char *szName );
	CBaseHighlighter( CBaseBBox &parentBox, float rX, float rY, const char *szName );
	CBaseHighlighter( const CBaseHighlighter &other );

	void Swap( CBaseHighlighter &other, bool fSwapChildren = true );

	inline void AddChild( CBaseHighlighter *pChild )
	{
		m_vecpChildren.push_back( pChild );
	}

	bool RemoveChild( CBaseHighlighter *pChild );

	CBaseBBox *m_pParentBox;
	std::vector<CBaseHighlighter *> m_vecpChildren;

	float m_rPositionX = 0;	//Left X-position in % of parent's width
	float m_rPositionY = 0;	//Top Y-position in % of parent's height
};