#include "Line.h"

void CLine::TransferOwnership( unsigned int uLevel )
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
		m_dPositionX2 = m_pParentBox->m_dPositionX + m_pParentBox->m_dWidth * m_dPositionX2;
		m_dPositionY2 = m_pParentBox->m_dPositionY + m_pParentBox->m_dHeight * m_dPositionY2;
		m_pParentBox = m_pParentBox->m_pParentBox;
	}
	m_pParentBox->AddChild( this );
}

void CLine::TransferOwnership( CBaseBBox &parentBox )
{
	m_pParentBox->RemoveChild( this );
	m_dPositionX = ( GetPositionX( ) - parentBox.GetPositionX( ) ) / (double) parentBox.GetWidth( );
	m_dPositionY = ( GetPositionY( ) - parentBox.GetPositionY( ) ) / (double) parentBox.GetHeight( );
	m_dPositionX2 = ( GetPositionX2( ) - parentBox.GetPositionX( ) ) / (double) parentBox.GetWidth( );
	m_dPositionY2 = ( GetPositionY2( ) - parentBox.GetPositionY( ) ) / (double) parentBox.GetHeight( );
	m_pParentBox = &parentBox;
	m_pParentBox->AddChild( this );
}