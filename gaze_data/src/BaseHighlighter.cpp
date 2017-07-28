#include "BaseHighlighter.h"
#include "BaseBBox.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

CBaseHighlighter::~CBaseHighlighter( void )
{
	if( m_pParentBox )
		m_pParentBox->RemoveChild( this );
	if( !m_vecpChildren.size( ) )
		return;

	fprintf( stderr, "Error destructing \"%s\"; The following child elements would be invalid:\n", szName );
	for( std::vector<CBaseHighlighter *>::iterator it = m_vecpChildren.begin( ); it < m_vecpChildren.end( ); it++ )
		fprintf( stderr, "  \"%s\"\n", ( *it )->szName );

	assert( !m_vecpChildren.size( ) );
}

unsigned int CBaseHighlighter::GetPositionX( unsigned int uLevel ) const
{
	register unsigned int uPositionX = m_pParentBox->GetPositionX( uLevel );
	register unsigned int uWidth = m_pParentBox->GetWidth( uLevel );
	return uPositionX + (unsigned int ) ( uWidth * m_rPositionX );
}

unsigned int CBaseHighlighter::GetPositionY( unsigned int uLevel ) const
{
	register unsigned int uPositionY = m_pParentBox->GetPositionY( uLevel );
	register unsigned int uHeight = m_pParentBox->GetHeight( uLevel );
	return uPositionY + (unsigned int ) ( uHeight * m_rPositionY );
}

CImage *CBaseHighlighter::GetImage( unsigned int uLevel )
{
	return m_pParentBox->GetImage( uLevel );
}

CBaseBBox *CBaseHighlighter::GetParent( unsigned int uLevel )
{
	if( !m_pParentBox || !uLevel-- )
	{
		fprintf( stderr, "Warning: Attempt to fetch a box from a lower level highlighter\n" );
		return nullptr;
	}

	return m_pParentBox->GetParent( uLevel );
}

void CBaseHighlighter::TransferOwnership( unsigned int uLevel )
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

CBaseHighlighter::CBaseHighlighter( void ) :
	szName{ "Unassigned" },
	m_pParentBox( nullptr )
{

}

CBaseHighlighter::CBaseHighlighter( const char *szName ) :
	szName{ 0 },
	m_pParentBox( nullptr )
{
	assert( szName != nullptr && strlen( szName ) );
	strncpy( const_cast<char *>( this->szName ), szName, 31 );
}

CBaseHighlighter::CBaseHighlighter( CBaseBBox &parentBox, unsigned int uX, unsigned int uY, unsigned int uLevel, const char *szName ) :
	szName{ 0 },
	m_pParentBox( &parentBox ),
	m_rPositionX( uX / (float) parentBox.GetWidth( uLevel ) ),
	m_rPositionY( uY / (float) parentBox.GetHeight( uLevel ) )
{
	assert( szName != nullptr && strlen( szName ) );
	strncpy( const_cast<char *>( this->szName ), szName, 31 );
	m_pParentBox->AddChild( this );

	assert( m_rPositionX >= 0.0 && m_rPositionX <= 1.0 );
	assert( m_rPositionY >= 0.0 && m_rPositionY <= 1.0 );
}

CBaseHighlighter::CBaseHighlighter( CBaseBBox &parentBox, float rX, float rY, const char *szName ) :
	szName{ 0 },
	m_pParentBox( &parentBox ),
	m_rPositionX( rX ),
	m_rPositionY( rY )
{
	assert( szName != nullptr && strlen( szName ) );
	strncpy( const_cast<char *>( this->szName ), szName, 31 );
	m_pParentBox->AddChild( this );

	assert( m_rPositionX >= 0.0 && m_rPositionX <= 1.0 );
	assert( m_rPositionY >= 0.0 && m_rPositionY <= 1.0 );
}

CBaseHighlighter::CBaseHighlighter( const CBaseHighlighter &other ) :
	szName{ 0 },
	m_pParentBox( other.m_pParentBox ),
	m_rPositionX( other.m_rPositionX ),
	m_rPositionY( other.m_rPositionY )
{
	assert( other.szName != nullptr && strlen( other.szName ) );
	strncpy( const_cast<char *>( this->szName ), other.szName, 31 );
	m_pParentBox->AddChild( this );
}

void CBaseHighlighter::Swap( CBaseHighlighter &other, bool fSwapChildren )
{
	{
		char szTempName[ 32 ];
		memcpy( szTempName, szName, 32 );
		memcpy( const_cast<char *>( szName ), other.szName, 32 );
		memcpy( const_cast<char *>( other.szName ), szTempName, 32 );
	}

	if( m_pParentBox )
		m_pParentBox->RemoveChild( this );
	if( other.m_pParentBox )
		other.m_pParentBox->RemoveChild( &other );

	std::swap( m_pParentBox, other.m_pParentBox );
	if( fSwapChildren )
	{
		m_vecpChildren.swap( other.m_vecpChildren );
		for( std::vector<CBaseHighlighter *>::iterator it = m_vecpChildren.begin( ); it < m_vecpChildren.end( ); it++ )
			( *it )->m_pParentBox = GetParent( 0 );
		for( std::vector<CBaseHighlighter *>::iterator it = other.m_vecpChildren.begin( ); it < other.m_vecpChildren.end( ); it++ )
			( *it )->m_pParentBox = other.GetParent( 0 );
	}
	std::swap( m_rPositionX, other.m_rPositionX );
	std::swap( m_rPositionY, other.m_rPositionY );

	if( m_pParentBox )
		m_pParentBox->AddChild( this );
	if( other.m_pParentBox )
		other.m_pParentBox->AddChild( &other );
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