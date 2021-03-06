#include "BaseHighlighter.h"
#include "BaseBBox.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#pragma warning( push )
#pragma warning( disable: 4996 )

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
	return uPositionX + (unsigned int ) ( uWidth * m_dPositionX );
}

unsigned int CBaseHighlighter::GetPositionY( unsigned int uLevel ) const
{
	register unsigned int uPositionY = m_pParentBox->GetPositionY( uLevel );
	register unsigned int uHeight = m_pParentBox->GetHeight( uLevel );
	return uPositionY + (unsigned int ) ( uHeight * m_dPositionY );
}

double CBaseHighlighter::GetRelPositionX( unsigned int uLevel ) const
{
	if( !uLevel-- )
		return m_dPositionX;

	register double dPositionX = m_pParentBox->GetRelPositionX( uLevel );
	register double dWidth = m_pParentBox->GetRelWidth( uLevel );
	return dPositionX + dWidth * m_dPositionX;
}

double CBaseHighlighter::GetRelPositionY( unsigned int uLevel ) const
{
	if( !uLevel-- )
		return m_dPositionY;

	register double dPositionY = m_pParentBox->GetRelPositionY( uLevel );
	register double dHeight = m_pParentBox->GetRelHeight( uLevel );
	return dPositionY + dHeight * m_dPositionY;
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

void CBaseHighlighter::Shift( const CVector<2> &vec2Offset )
{
	m_dPositionX += vec2Offset[ 0 ];
	m_dPositionY += vec2Offset[ 1 ];
	m_dPositionX = m_dPositionX >= 0 ? m_dPositionX : 0;
	m_dPositionX = m_dPositionX <= 1 ? m_dPositionX : 1;
	m_dPositionY = m_dPositionY >= 0 ? m_dPositionY : 0;
	m_dPositionY = m_dPositionY <= 1 ? m_dPositionY : 1;
}

void CBaseHighlighter::MakeInvalid( void )
{
	if( m_pParentBox )
		m_pParentBox->RemoveChild( this );

	if( !m_vecpChildren.size( ) )
	{
		strncpy( const_cast<char *>( this->szName ), "Unassigned", 31 );
		return;
	}

	fprintf( stderr, "Error invalidating \"%s\"; The following child elements would be invalid:\n", szName );
	for( std::vector<CBaseHighlighter *>::iterator it = m_vecpChildren.begin( ); it < m_vecpChildren.end( ); it++ )
		fprintf( stderr, "  \"%s\"\n", ( *it )->szName );

	assert( !m_vecpChildren.size( ) );
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

		m_dPositionX = m_pParentBox->m_dPositionX + m_pParentBox->m_dWidth * m_dPositionX;
		m_dPositionY = m_pParentBox->m_dPositionY + m_pParentBox->m_dHeight * m_dPositionY;
		m_pParentBox = m_pParentBox->m_pParentBox;
	}
	m_pParentBox->AddChild( this );
}

void CBaseHighlighter::TransferOwnership( CBaseBBox &parentBox )
{
	m_pParentBox->RemoveChild( this );
	m_dPositionX = ( GetPositionX( ) - parentBox.GetPositionX( ) ) / (double) parentBox.GetWidth( );
	m_dPositionY = ( GetPositionY( ) - parentBox.GetPositionY( ) ) / (double) parentBox.GetHeight( );
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
	assert( szName != nullptr && strlen( szName ) && strlen( szName ) < 32 );
	strncpy( const_cast<char *>( this->szName ), szName, 31 );
}

CBaseHighlighter::CBaseHighlighter( CBaseBBox &parentBox, unsigned int uX, unsigned int uY, unsigned int uLevel, const char *szName ) :
	szName{ 0 },
	m_pParentBox( &parentBox ),
	m_dPositionX( uX / (double) parentBox.GetWidth( uLevel ) ),
	m_dPositionY( uY / (double) parentBox.GetHeight( uLevel ) )
{
	assert( szName != nullptr && strlen( szName ) && strlen( szName ) < 32 );
	strncpy( const_cast<char *>( this->szName ), szName, 31 );
	m_pParentBox->AddChild( this );

	assert( m_dPositionX >= 0.0 && m_dPositionX <= 1.0 );
	assert( m_dPositionY >= 0.0 && m_dPositionY <= 1.0 );
}

CBaseHighlighter::CBaseHighlighter( CBaseBBox &parentBox, double dX, double dY, const char *szName ) :
	szName{ 0 },
	m_pParentBox( &parentBox ),
	m_dPositionX( dX ),
	m_dPositionY( dY )
{
	assert( szName != nullptr && strlen( szName ) && strlen( szName ) < 32 );
	strncpy( const_cast<char *>( this->szName ), szName, 31 );
	m_pParentBox->AddChild( this );

	assert( m_dPositionX >= 0.0 && m_dPositionX <= 1.0 );
	assert( m_dPositionY >= 0.0 && m_dPositionY <= 1.0 );
}

CBaseHighlighter::CBaseHighlighter( CBaseBBox &parentBox, const CVector<2> &vec2Pos, const char *szName ) :
	szName{ 0 },
	m_pParentBox( &parentBox ),
	m_dPositionX( vec2Pos[ 0 ] ),
	m_dPositionY( vec2Pos[ 1 ] )
{
	assert( szName != nullptr && strlen( szName ) && strlen( szName ) < 32 );
	strncpy( const_cast<char *>( this->szName ), szName, 31 );
	m_pParentBox->AddChild( this );

	assert( m_dPositionX >= 0.0 && m_dPositionX <= 1.0 );
	assert( m_dPositionY >= 0.0 && m_dPositionY <= 1.0 );
}

CBaseHighlighter::CBaseHighlighter( const CBaseHighlighter &other ) :
	szName{ 0 },
	m_pParentBox( other.m_pParentBox ),
	m_dPositionX( other.m_dPositionX ),
	m_dPositionY( other.m_dPositionY )
{
	assert( other.szName != nullptr && strlen( other.szName ) && strlen( other.szName ) < 32 );
	strncpy( const_cast<char *>( this->szName ), other.szName, 31 );
	if( m_pParentBox )
		m_pParentBox->AddChild( this );
}

CBaseHighlighter & CBaseHighlighter::operator=( const CBaseHighlighter &other )
{
	operator=( std::move( CBaseHighlighter( other ) ) );
	return *this;
}

CBaseHighlighter::CBaseHighlighter( CBaseHighlighter &&other ) :
	szName{ 0 },
	m_pParentBox( other.m_pParentBox ),
	m_vecpChildren( std::move( other.m_vecpChildren ) ),
	m_dPositionX( other.m_dPositionX ),
	m_dPositionY( other.m_dPositionY )
{
	assert( other.szName != nullptr && strlen( other.szName ) && strlen( other.szName ) < 32 );
	strncpy( const_cast<char *>( this->szName ), other.szName, 31 );
	strncpy( const_cast<char *>( other.szName ), "Unassigned", 31 );

	if( m_pParentBox )
	{
		m_pParentBox->RemoveChild( &other );
		m_pParentBox->AddChild( this );
	}
	for( auto pChild : m_vecpChildren )
		pChild->m_pParentBox = static_cast<CBaseBBox *>( this );	//If the BaseHighlighter has children it must be of type CBaseBBox!
}

CBaseHighlighter &CBaseHighlighter::operator=( CBaseHighlighter &&other )
{
	assert( !m_vecpChildren.size( ) );	//Children would become invalid on move
	if( m_pParentBox )
		m_pParentBox->RemoveChild( this );

	m_pParentBox = other.m_pParentBox;
	m_vecpChildren = std::move( other.m_vecpChildren );
	m_dPositionX = other.m_dPositionX;
	m_dPositionY = other.m_dPositionY;

	assert( other.szName != nullptr && strlen( other.szName ) && strlen( other.szName ) < 32 );
	strncpy( const_cast<char *>( this->szName ), other.szName, 31 );
	strncpy( const_cast<char *>( other.szName ), "Unassigned", 31 );

	if( m_pParentBox )
	{
		m_pParentBox->RemoveChild( &other );
		m_pParentBox->AddChild( this );
	}

	for( auto pChild : m_vecpChildren )
		pChild->m_pParentBox = static_cast<CBaseBBox *>( this );	//If the BaseHighlighter has children it must be of type CBaseBBox!

	return *this;
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

#pragma warning( pop )