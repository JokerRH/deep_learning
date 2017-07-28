#include "Image.h"
#include "BBox.h"

void CImage::Crop( CBBox &box, unsigned int uLevel )
{
	register cv::Rect rect( box.GetPositionX( uLevel ), box.GetPositionY( uLevel ), box.GetWidth( uLevel ), box.GetHeight( uLevel ) );
	m_pParentBox = box.GetImage( uLevel );
	m_rWidth = rect.width / (float) m_pParentBox->GetWidth( 0 );
	m_rHeight = rect.height / (float) m_pParentBox->GetHeight( 0 );
	m_rPositionX = rect.x / (float) m_pParentBox->GetWidth( 0 );
	m_rPositionY = rect.y / (float) m_pParentBox->GetHeight( 0 );
	
	matImage = ( m_pParentBox->GetImage( 0 )->matImage )( rect ).clone( );
}