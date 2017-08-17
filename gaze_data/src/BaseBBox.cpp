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

		m_dPositionX = m_pParentBox->m_dPositionX + m_pParentBox->m_dWidth * m_dPositionX;
		m_dPositionY = m_pParentBox->m_dPositionY + m_pParentBox->m_dHeight * m_dPositionY;
		m_dWidth *= m_pParentBox->m_dWidth;
		m_dHeight *= m_pParentBox->m_dHeight;
		m_pParentBox = m_pParentBox->m_pParentBox;
	}
	m_pParentBox->AddChild( this );
}

void CBaseBBox::TransferOwnership( CBaseBBox &parentBox )
{
	m_pParentBox->RemoveChild( this );
	m_dPositionX = ( GetPositionX( ) - parentBox.GetPositionX( ) ) / (double) parentBox.GetWidth( );
	m_dPositionY = ( GetPositionY( ) - parentBox.GetPositionY( ) ) / (double) parentBox.GetHeight( );
	m_dWidth = GetWidth( ) / (double) parentBox.GetWidth( );
	m_dHeight = GetHeight( ) / (double) parentBox.GetHeight( );
	m_pParentBox = &parentBox;
	m_pParentBox->AddChild( this );
}

CBaseBBox::CBaseBBox( const char *szName ) :
	CBaseHighlighter( szName )
{

}

CBaseBBox::CBaseBBox( CBaseBBox &parentBox, unsigned int uX, unsigned int uY, unsigned int uWidth, unsigned int uHeight, unsigned int uLevel, const char *szName ) :
	CBaseHighlighter( parentBox, uX, uY, uLevel, szName ),
	m_dWidth( uWidth / (double) parentBox.GetWidth( uLevel ) ),
	m_dHeight( uHeight / (double) parentBox.GetHeight( uLevel ) )
{
	assert( m_dWidth >= 0.0 && m_dPositionX + m_dWidth <= 1.0 );
	assert( m_dHeight >= 0.0 && m_dPositionY + m_dHeight <= 1.0 );
}

CBaseBBox::CBaseBBox( CBaseBBox &parentBox, double dX, double dY, double dWidth, double dHeight, const char *szName ) :
	CBaseHighlighter( parentBox, dX, dY, szName ),
	m_dWidth( dWidth ),
	m_dHeight( dHeight )
{
	assert( m_dWidth >= 0.0 && m_dPositionX + m_dWidth <= 1.0 );
	assert( m_dHeight >= 0.0 && m_dPositionY + m_dHeight <= 1.0 );
}