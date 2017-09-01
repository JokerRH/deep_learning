/*!
	\file Queue.cpp
	\brief		Implementations for Queue
	\author		Rainer Heinelt
	\details	Implements definitions for Queues, used to buffer data.
*/
#include "Queue.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/*!
	\param[in]	uCount	Number of buffers to create
	\param[in]	uSize	Size of each buffer
	\brief				Creates an instance of the CStorage class.
	\details			Allocates memory for buffers.
*/
CStorage::CStorage( unsigned int uCount, unsigned int uSize ) :
	m_uCount( uCount )
{
	pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, nullptr );

	m_pRawData = new char[ uCount * uSize ];
	m_apStorageData = new char *[ uCount ];

	for( unsigned int u = 0; u < uCount; u++ )
		m_apStorageData[ u ] = m_pRawData + u * uSize;

	pthread_mutex_init( &m_mtxData, NULL );
	pthread_cond_init( &m_condWait, NULL );
	
	pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, nullptr );
}

/*!
	\brief		Destroys an instance of the CStorage class.
	\details	Deallocates memory for buffers.
*/
CStorage::~CStorage( void ) noexcept
{
	pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, nullptr );
	
	pthread_cond_destroy( &m_condWait );
	pthread_mutex_destroy( &m_mtxData );
	delete[] m_apStorageData;
	delete[] m_pRawData;
	
	pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, nullptr );
}

/*!
	\return		Returns pointer to a buffer
	\brief		Fetches pointer to a buffer
	\details	Fetches pointer to a buffer.
	If no buffer is available waits until one is released.
*/
void *CStorage::GetPointer( void )
{
	char *pReturn;
	pthread_cleanup_push( (void (*)(void*)) pthread_mutex_unlock, (void *) &m_mtxData );
		pthread_mutex_lock( &m_mtxData );
		
		while( m_uCurrent >= m_uCount )
			pthread_cond_wait( &m_condWait, &m_mtxData );
		
		//Fetch pointer
		pReturn = m_apStorageData[ m_uCurrent++ ];
	pthread_cleanup_pop( 1 );
	
	return pReturn;
}

/*!
	\return		Returns pointer to a buffer or nullptr
	\brief		Fetches pointer to a buffer
	\details	Fetches pointer to a buffer.
	If no buffer is available returns nullptr.
*/
void *CStorage::GetPointer_Try( void )
{
	char *pReturn = nullptr;
	pthread_cleanup_push( (void (*)(void*)) pthread_mutex_unlock, (void *) &m_mtxData );
		pthread_mutex_lock( &m_mtxData );
		do
		{
			if( m_uCurrent >= m_uCount )
				break;	//return nullptr
			
			//Fetch pointer
			pReturn = m_apStorageData[ m_uCurrent++ ];
		} while( 0 );
	pthread_cleanup_pop( 1 );

	return pReturn;
}

/*!
	\param[in]	pData	Buffer to be released
	\brief				Releases the buffer referenced by \a pData.
	\warning			Do not use the buffer after it has been released.
	\warning			There are no checks wether the buffer was originaly fetched from this instance or not.
*/
void CStorage::ReleasePointer( void *pData )
{
	bool fThrow = false;
	
	pthread_cleanup_push( (void (*)(void*)) pthread_mutex_unlock, (void *) &m_mtxData );
		pthread_mutex_lock( &m_mtxData );

		if( m_uCurrent )
		{
			m_apStorageData[ --m_uCurrent ] = static_cast<char *>( pData );
			pthread_cond_signal( &m_condWait );
		}
		else
			fThrow = true;
	pthread_cleanup_pop( 1 );
	
	if( fThrow )
	{
		fprintf( stderr, "[Storage] More pointers were released than originally existed!\n" );
		assert( false );
	}
}