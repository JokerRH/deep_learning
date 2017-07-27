#pragma once

#include <assert.h>
#include <vector>

class CImage;
class CBaseBBox;
class CBaseHighlighter
{
public:
	~CBaseHighlighter( void );
	virtual inline unsigned int GetPositionX( unsigned int uLevel = -1 ) const
	{
		register unsigned int uPositionX = m_pParentBox->GetPositionX( uLevel );
		register unsigned int uWidth = m_pParentBox->GetWidth( uLevel );
		return uPositionX + (unsigned int ) ( uWidth * m_rPositionX );
	}

	virtual inline unsigned int GetPositionY( unsigned int uLevel = -1 ) const
	{
		register unsigned int uPositionY = m_pParentBox->GetPositionY( uLevel );
		register unsigned int uHeight = m_pParentBox->GetHeight( uLevel );
		return uPositionY + (unsigned int ) ( uHeight * m_rPositionY );
	}

	virtual inline CImage *GetImage( unsigned int uLevel = -1 )
	{
		return m_pParentBox->GetImage( uLevel );
	}

	inline CBaseBBox *GetParent( unsigned int uLevel = -1 )
	{
		if( !m_pParentBox || !uLevel-- )
		{
			fprintf( stderr, "Warning: Attempt to fetch a box from a lower level highlighter\n" );
			return nullptr;
		}

		return m_pParentBox->GetParent( uLevel );
	}

	virtual void TransferOwnership( unsigned int uLevel = 1 );
	virtual void TransferOwnership( CBaseBBox &parentBox );

	const char szName[ 32 ];

protected:
	CBaseHighlighter( const char *szName );
	CBaseHighlighter( CBaseBBox &parentBox, unsigned int uX, unsigned int uY, unsigned int uLevel, const char *szName );
	CBaseHighlighter( const CBaseHighlighter &other );

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