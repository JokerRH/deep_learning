#pragma once

#include <assert.h>
#include "BaseHighlighter.h"

class CBaseBBox : public CBaseHighlighter
{
public:
	virtual inline unsigned int GetWidth( unsigned int uLevel = -1 ) const
	{
		register unsigned int uWidth = m_pParentBox->GetWidth( uLevel );
		return (unsigned int) ( uWidth * m_rWidth );
	}

	virtual inline unsigned int GetHeight( unsigned int uLevel = -1 ) const
	{
		register unsigned int uHeight = m_pParentBox->GetHeight( uLevel );
		return (unsigned int) ( uHeight * m_rHeight );
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
	CBaseBBox( CBaseBBox &parentBox, float rX, float rY, float rWidth, float rHeight, const char *szName );
	
	void Swap( CBaseBBox &other, bool fSwapChildren = true );

	float m_rWidth = 1;		///Width in % of parent's width
	float m_rHeight = 1;	///Height in % of parent's height
	
	friend CBaseHighlighter;
};

#if 0
	virtual inline float GetRelPositionX( unsigned int uLevel = 0 ) const
	{
		if( !m_pParentBox || !uLevel-- )
			return m_rPositionX;

		register float rPositionX = m_pParentBox->GetRelPositionX( uLevel );
		register float rWidth = m_pParentBox->GetRelWidth( uLevel );
		return rPositionX + rWidth * m_rPositionX;
	}

	virtual inline float GetRelPositionY( unsigned int uLevel = 0 ) const
	{
		if( !m_pParentBox || !uLevel-- )
			return m_rPositionY;

		register float rPositionY = m_pParentBox->GetRelPositionY( uLevel );
		register float rHeight = m_pParentBox->GetRelHeight( uLevel );
		return rPositionY + rHeight * m_rPositionY;
	}

	virtual inline float GetRelWidth( unsigned int uLevel = 0 ) const
	{
		if( !m_pParentBox || !uLevel-- )
			return m_rWidth;

		register float rWidth = m_pParentBox->GetRelWidth( uLevel );
		return rWidth * m_rWidth;
	}

	virtual inline float GetRelHeight( unsigned int uLevel = 0 ) const
	{
		if( !m_pParentBox || !uLevel-- )
			return m_rHeight;

		register float rHeight = m_pParentBox->GetRelHeight( uLevel );
		return rHeight * m_rHeight;
	}

	inline CBaseBBox( float rX, float rY, float rWidth, float rHeight, CBaseBBox &baseBBox, const char *szName ) :
		m_pParentBox( &baseBBox ),
		m_rPositionX( rX ),
		m_rPositionY( rY ),
		m_rWidth( rWidth ),
		m_rHeight( rHeight )
	{
		strncpy( m_szName, szName, 32 );
		m_szName[ 31 ] = 0;
	}
#endif