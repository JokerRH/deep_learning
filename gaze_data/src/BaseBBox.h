#pragma once

#include <assert.h>
#include "BaseHighlighter.h"
#include "Render/Vector.h"

class CImage;
class CLine;
class CBaseBBox : public CBaseHighlighter
{
public:
	virtual unsigned int GetWidth( unsigned int uLevel = -1 ) const;
	virtual unsigned int GetHeight( unsigned int uLevel = -1 ) const;
	virtual double GetRelWidth( unsigned int uLevel = -1 ) const;
	virtual double GetRelHeight( unsigned int uLevel = -1 ) const;
	CBaseBBox *GetParent( unsigned int uLevel = -1 ) override;
	CVector<2> GetCenter( void ) const;
	
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
	friend CLine;
	friend CImage;
};

inline unsigned int CBaseBBox::GetWidth( unsigned int uLevel ) const
{
	register unsigned int uWidth = m_pParentBox->GetWidth( uLevel );
	return (unsigned int) ( uWidth * m_dWidth );
}

inline unsigned int CBaseBBox::GetHeight( unsigned int uLevel ) const
{
	register unsigned int uHeight = m_pParentBox->GetHeight( uLevel );
	return (unsigned int) ( uHeight * m_dHeight );
}

inline double CBaseBBox::GetRelWidth( unsigned int uLevel ) const
{
	if( !uLevel )
		return m_dWidth;

	register double dWidth = m_pParentBox->GetRelWidth( uLevel );
	return dWidth * m_dWidth;
}

inline double CBaseBBox::GetRelHeight( unsigned int uLevel ) const
{
	if( !uLevel )
		return m_dHeight;

	register double dHeight = m_pParentBox->GetRelHeight( uLevel );
	return dHeight * m_dHeight;
}

inline CBaseBBox *CBaseBBox::GetParent( unsigned int uLevel )
{
	if( !m_pParentBox || !uLevel-- )
		return this;

	return m_pParentBox->GetParent( uLevel );
}

inline CVector<2> CBaseBBox::GetCenter( void ) const
{
	return CVector<2>( { m_dPositionX + m_dWidth / 2, m_dPositionY + m_dHeight / 2 } );
}