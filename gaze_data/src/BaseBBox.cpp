#include "BaseBBox.h"

void CBaseBBox::TransferOwnership( unsigned int uLevel = 1 )
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
	m_rWidth = GetWidth( ) / (float) baseBBox.GetWidth( );
	m_rHeight = GetHeight( ) / (float) baseBBox.GetHeight( );
	m_pParentBox = &parentBox;
	m_pParentBox->AddChild( this );
}