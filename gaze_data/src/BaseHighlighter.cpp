#include "BaseHighlighter.h"
#include "BaseBBox.h"

~CBaseHighlighter( void )
{
	m_pParentBox->RemoveChild( this );
	if( !m_vecpChildren.size( ) )
		return;

	fprintf( stderr, "Warning: Destructing \"%s\"; The following child elements will be invalid:\n" );
	for( std::vector<CBaseHighlighter *>::iterator it = m_vecpChildren.begin( ); it < m_vecpChildren.end( ); it++ )
		fprintf( "  %s\n", *it->szName );
}

void CBaseHighlighter::TransferOwnership( unsigned int uLevel = 1 )
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
		m_pParentBox = m_pParentBox->m_pParentBox;
	}
	m_pParentBox->AddChild( this );
}

void CBaseHighlighter::TransferOwnership( CBaseBBox &parentBox )
{
	m_pParentBox->RemoveChild( this );
	m_rPositionX = ( GetPositionX( ) - parentBox.GetPositionX( ) ) / (float) parentBox.GetWidth( );
	m_rPositionY = ( GetPositionY( ) - parentBox.GetPositionY( ) ) / (float) parentBox.GetHeight( );
	m_pParentBox = &parentBox;
	m_pParentBox->AddChild( this );
}

CBaseHighlighter::CBaseHighlighter( const char *szName );
{
	assert( szName != nullptr );
	strncpy( const_cast<char *>( this->szName ), szName, 32 );
	const_cast<char *>( this->szName )[ 31 ] = 0;
}

CBaseHighlighter::CBaseHighlighter( CBaseBBox &parentBox, unsigned int uX, unsigned int uY, unsigned int uLevel, const char *szName ) :
	m_pParentBox( &parentBox ),
	m_rPositionX( uX / (float) parentBox.GetWidth( uLevel ) ),
	m_rPositionY( uY / (float) parentBox.GetHeight( uLevel ) ),
{
	assert( szName != nullptr );
	strncpy( const_cast<char *>( this->szName ), szName, 32 );
	const_cast<char *>( this->szName )[ 31 ] = 0;
	m_pParentBox->AddChild( this );

	assert( m_rPositionX >= 0.0 && m_rPositionX <= 1.0 );
	assert( m_rPositionY >= 0.0 && m_rPositionY <= 1.0 );
}

CBaseHighlighter::CBaseHighlighter( const CBaseHighlighter &other ) :
	m_pParentBox( other.m_pParentBox ),
	szName{ other.szName },
	m_rPositionX( other.m_rPositionX ),
	m_rPositionY( other.m_rPositionY )
{
	m_pParentBox->AddChild( this );
}

bool CBaseHighlighter::RemoveChild( CBaseHighlighter *pChild )
{
	for( std::vector<CBaseHighlighter *>::iterator it = m_vecpChildren.begin( ); it < m_vecpChildren.end( ); it++ )
		if( *it == pChild )
		{
			m_vecpChildren.erase( it );
			return true;
		}

	return false;
}