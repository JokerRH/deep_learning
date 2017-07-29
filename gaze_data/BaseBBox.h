#pragma once

#include <assert.h>
#include "BaseHighlighter.h"

class CBaseBBox : public CBaseHighlighter
{
public:
	virtual inline unsigned int GetWidth( unsigned int uLevel = -1 ) const
	{
		register unsigned int uWidth = m_pParentBox->GetWidth( uLevel );
		return (unsigned int) ( uWidth * m_dWidth );
	}

	virtual inline unsigned int GetHeight( unsigned int uLevel = -1 ) const
	{
		register unsigned int uHeight = m_pParentBox->GetHeight( uLevel );
		return (unsigned int) ( uHeight * m_dHeight );
	}

	inline CBaseBBox *GetParent( unsigned int uLevel = -1 ) override
	{
		if( !m_pParentBox || !uLevel-- )
			return this;

		return m_pParentBox->GetParent( uLevel );
	}
	
	virtual void TransferOwnership( unsigned int uLevel = 1 );
	virtual void TransferOwnership( CBaseBBox &parentBox );

protected:
	CBaseBBox( void )
	{

	}

	CBaseBBox( const char *szName );
	CBaseBBox( const CBaseBBox &other );
	CBaseBBox( CBaseBBox &parentBox, unsigned int uX, unsigned int uY, unsigned int uWidth, unsigned int uHeight, unsigned int uLevel, const char *szName );
	CBaseBBox( CBaseBBox &parentBox, double dX, double dY, double dWidth, double dHeight, const char *szName );
	
	void Swap( CBaseBBox &other, bool fSwapChildren = true );

	double m_dWidth = 1;	///Width in % of parent's width
	double m_dHeight = 1;	///Height in % of parent's height
	
	friend CBaseHighlighter;
};