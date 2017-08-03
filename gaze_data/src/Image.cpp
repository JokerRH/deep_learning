#include "Image.h"
#include "BBox.h"

void CImage::Crop( CBBox &box, unsigned int uLevel )
{
	register cv::Rect rect( box.GetPositionX( uLevel ), box.GetPositionY( uLevel ), box.GetWidth( uLevel ), box.GetHeight( uLevel ) );
	m_pParentBox = box.GetImage( uLevel );
	m_dWidth = rect.width / (double) m_pParentBox->GetWidth( 0 );
	m_dHeight = rect.height / (double) m_pParentBox->GetHeight( 0 );
	m_dPositionX = rect.x / (double) m_pParentBox->GetWidth( 0 );
	m_dPositionY = rect.y / (double) m_pParentBox->GetHeight( 0 );
	
	matImage = ( m_pParentBox->GetImage( 0 )->matImage )( rect ).clone( );
}