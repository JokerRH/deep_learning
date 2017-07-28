#include "BaseBBox.h"

void CBaseBBox::TransferOwnership( unsigned int uLevel )
{
	if( !uLevel || !m_pParentBox->m_pParentBox )
		return;

	m_pParentBox->RemoveChild( this );
	while( uLevel-- )
	{
		if( !m_pParentBox->m_pParentBox )
			break;	//Parent is top level image

		m_rPositionX = m_pParentBox->m_rPositionX + m_pParentBox->m_rWidth * m_rPositionX;
		m_rPositionY = m_pParentBox->m_rPositionY + m_pParentBox->m_rHeight * m_rPositionY;
		m_rWidth *= m_pParentBox->m_rWidth;
		m_rHeight *= m_pParentBox->m_rHeight;
		m_pParentBox = m_pParentBox->m_pParentBox;
	}
	m_pParentBox->AddChild( this );
}

void CBaseBBox::TransferOwnership( CBaseBBox &parentBox )
{
	m_pParentBox->RemoveChild( this );
	m_rPositionX = ( GetPositionX( ) - parentBox.GetPositionX( ) ) / (float) parentBox.GetWidth( );
	m_rPositionY = ( GetPositionY( ) - parentBox.GetPositionY( ) ) / (float) parentBox.GetHeight( );
	m_rWidth = GetWidth( ) / (float) parentBox.GetWidth( );
	m_rHeight = GetHeight( ) / (float) parentBox.GetHeight( );
	m_pParentBox = &parentBox;
	m_pParentBox->AddChild( this );
}

CBaseBBox::CBaseBBox( const char *szName ) :
	CBaseHighlighter( szName )
{

}

CBaseBBox::CBaseBBox( const CBaseBBox &other ) :
	CBaseHighlighter( other ),
	m_rWidth( other.m_rWidth ),
	m_rHeight( other.m_rHeight )
{

}

CBaseBBox::CBaseBBox( CBaseBBox &parentBox, unsigned int uX, unsigned int uY, unsigned int uWidth, unsigned int uHeight, unsigned int uLevel, const char *szName ) :
	CBaseHighlighter( parentBox, uX, uY, uLevel, szName ),
	m_rWidth( uWidth / (float) parentBox.GetWidth( uLevel ) ),
	m_rHeight( uHeight / (float) parentBox.GetHeight( uLevel ) )
{
	assert( m_rWidth >= 0.0 && m_rPositionX + m_rWidth <= 1.0 );
	assert( m_rHeight >= 0.0 && m_rPositionY + m_rHeight <= 1.0 );
}

CBaseBBox::CBaseBBox( CBaseBBox &parentBox, float rX, float rY, float rWidth, float rHeight, const char *szName ) :
	CBaseHighlighter( parentBox, rX, rY, szName ),
	m_rWidth( rWidth ),
	m_rHeight( rHeight )
{
	assert( m_rWidth >= 0.0 && m_rPositionX + m_rWidth <= 1.0 );
	assert( m_rHeight >= 0.0 && m_rPositionY + m_rHeight <= 1.0 );
}

void CBaseBBox::Swap( CBaseBBox &other, bool fSwapChildren )
{
	CBaseHighlighter::Swap( other, fSwapChildren );
	std::swap( m_rWidth, other.m_rWidth );
	std::swap( m_rHeight, other.m_rHeight );
}