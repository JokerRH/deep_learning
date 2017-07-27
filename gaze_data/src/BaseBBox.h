#pragma once

class CImage;
class CPoint;

class CBaseBBox
{
public:
	virtual inline unsigned int GetPositionX( unsigned int uLevel = -1 ) const
	{
		register unsigned int uPositionX = m_pParentBox->GetPositionX( uLevel );
		register unsigned int uWidth = m_pParentBox->GetWidth( uLevel );
		return uPositionX + (unsigned int ) ( uWidth * m_rPositionX );
	}

	virtual inline unsigned int GetPositionY( unsigned int uLevel = -1 ) const
	{
		register unsigned int uPositionY = m_pParentBox->GetPositionY( uLevel );
		register unsigned int uHeight = m_pParentBox->GetHeight( uLevel );
		return uPositionY + (unsigned int ) ( uHeight * m_rPositionY );
	}

	virtual inline unsigned int GetWidth( unsigned int uLevel = -1 ) const
	{
		register unsigned int uWidth = m_pParentBox->GetWidth( uLevel );
		return (unsigned int) ( uWidth * m_rWidth );
	}

	virtual inline unsigned int GetHeight( unsigned int uLevel = -1 ) const
	{
		register unsigned int uHeight = m_pParentBox->GetHeight( uLevel );
		return (unsigned int) ( uHeight * m_rHeight );
	}
	
	virtual inline float GetRelPositionX( unsigned int uLevel = 0 ) const
	{
		if( !m_pParentBox || !uLevel-- )
			return m_rPositionX;

		register float rPositionX = m_pParentBox->GetRelPositionX( uLevel );
		register float rWidth = m_pParentBox->GetRelWidth( uLevel );
		return rPositionX + rWidth * m_rPositionX;
	}

	virtual inline float GetRelPositionY( unsigned int uLevel = 0 ) const
	{
		if( !m_pParentBox || !uLevel-- )
			return m_rPositionY;

		register float rPositionY = m_pParentBox->GetRelPositionY( uLevel );
		register float rHeight = m_pParentBox->GetRelHeight( uLevel );
		return rPositionY + rHeight * m_rPositionY;
	}

	virtual inline float GetRelWidth( unsigned int uLevel = 0 ) const
	{
		if( !m_pParentBox || !uLevel-- )
			return m_rWidth;

		register float rWidth = m_pParentBox->GetRelWidth( uLevel );
		return rWidth * m_rWidth;
	}

	virtual inline float GetRelHeight( unsigned int uLevel = 0 ) const
	{
		if( !m_pParentBox || !uLevel-- )
			return m_rHeight;

		register float rHeight = m_pParentBox->GetRelHeight( uLevel );
		return rHeight * m_rHeight;
	}
	
	virtual inline CImage *GetImage( unsigned int uLevel = -1 )
	{
		return m_pParentBox->GetImage( uLevel );
	}

	inline CBaseBBox *GetParent( unsigned int uLevel = -1 )
	{
		if( !m_pParentBox || !uLevel-- )
			return this;

		return m_pParentBox->GetParent( uLevel );
	}
	
	virtual inline void TransferOwnership( unsigned int uLevel = 1 )
	{
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
	}
	
	virtual inline void TransferOwnership( CBaseBBox &baseBBox )
	{
		m_rPositionX = ( GetPositionX( ) - baseBBox.GetPositionX( ) ) / (float) baseBBox.GetWidth( );
		m_rPositionY = ( GetPositionY( ) - baseBBox.GetPositionY( ) ) / (float) baseBBox.GetHeight( );
		m_rWidth = GetWidth( ) / (float) baseBBox.GetWidth( );
		m_rHeight = GetHeight( ) / (float) baseBBox.GetHeight( );
		m_pParentBox = &baseBBox;
	}
	
	inline const char *GetName( void )
	{
		return m_szName;
	}

protected:
	inline CBaseBBox( const char *szName ) :
		m_pParentBox( nullptr )
	{
		strncpy( m_szName, szName, 32 );
		m_szName[ 31 ] = 0;
	}
	
	inline CBaseBBox( const CBaseBBox &box )
	{
		memcpy( this, &box, sizeof( CBaseBBox ) );
	}

	inline CBaseBBox( CBaseBBox &baseBBox, unsigned int uX, unsigned int uY, unsigned int uWidth, unsigned int uHeight, const char *szName ) :
		m_pParentBox( &baseBBox ),
		m_rPositionX( uX / (float) baseBBox.GetWidth( ) ),
		m_rPositionY( uY / (float) baseBBox.GetHeight( ) ),
		m_rWidth( uWidth / (float) baseBBox.GetWidth( ) ),
		m_rHeight( uHeight / (float) baseBBox.GetHeight( ) )
	{
		strncpy( m_szName, szName, 32 );
		m_szName[ 31 ] = 0;
	}
	
	inline CBaseBBox( float rX, float rY, float rWidth, float rHeight, CBaseBBox &baseBBox, const char *szName ) :
		m_pParentBox( &baseBBox ),
		m_rPositionX( rX ),
		m_rPositionY( rY ),
		m_rWidth( rWidth ),
		m_rHeight( rHeight )
	{
		strncpy( m_szName, szName, 32 );
		m_szName[ 31 ] = 0;
	}

	CBaseBBox *m_pParentBox;
	char m_szName[ 32 ];
	float m_rPositionX = 0;	//Left X-position in % of parent's width
	float m_rPositionY = 0;	//Top Y-position in % of parent's height
	float m_rWidth = 1;		//Width in % of parent's width
	float m_rHeight = 1;	//Height in % of parent's height
	
	friend CPoint;
};