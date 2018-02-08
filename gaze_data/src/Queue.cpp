/*!
\file Queue.cpp
\brief		Implementations for Queue
\author		Rainer Heinelt
\details	Implements definitions for Queues, used to buffer data.
*/
#include "Queue.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

/*!
\param[in]	uCount	Number of buffers to create
\param[in]	uSize	Size of each buffer
\brief				Creates an instance of the CStorage class.
\details			Allocates memory for buffers.
*/
CStorage::CStorage(unsigned int uCount, unsigned int uSize) :
	m_uCount(uCount)
{
	m_pRawData = new char[uCount * uSize];
	m_apStorageData = new char *[uCount];

	for (unsigned int u = 0; u < uCount; u++)
		m_apStorageData[u] = m_pRawData + u * uSize;
}

/*!
\brief		Destroys an instance of the CStorage class.
\details	Deallocates memory for buffers.
*/
CStorage::~CStorage(void) noexcept
{
	delete[] m_apStorageData;
	delete[] m_pRawData;
}

/*!
\return		Returns pointer to a buffer
\brief		Fetches pointer to a buffer
\details	Fetches pointer to a buffer.
If no buffer is available waits until one is released.
*/
void *CStorage::GetPointer(void)
{
	char *pReturn;
	{
		std::unique_lock<std::mutex> lock(m_mtxData);

		while (m_uCurrent >= m_uCount)
			m_condWait.wait(lock);

		//Fetch pointer
		pReturn = m_apStorageData[m_uCurrent++];
	}

	return pReturn;
}

/*!
\return		Returns pointer to a buffer or nullptr
\brief		Fetches pointer to a buffer
\details	Fetches pointer to a buffer.
If no buffer is available returns nullptr.
*/
void *CStorage::GetPointer_Try(void)
{
	char *pReturn = nullptr;
	{
		std::unique_lock<std::mutex> lock(m_mtxData);
		do
		{
			if (m_uCurrent >= m_uCount)
				break;	//return nullptr

						//Fetch pointer
			pReturn = m_apStorageData[m_uCurrent++];
		} while (0);
	}

	return pReturn;
}

/*!
\param[in]	pData	Buffer to be released
\brief				Releases the buffer referenced by \a pData.
\warning			Do not use the buffer after it has been released.
\warning			There are no checks wether the buffer was originaly fetched from this instance or not.
*/
void CStorage::ReleasePointer(void *pData)
{
	bool fThrow = false;

	{
		std::unique_lock<std::mutex> lock(m_mtxData);

		if (m_uCurrent)
		{
			m_apStorageData[--m_uCurrent] = static_cast<char *>(pData);
			m_condWait.notify_one();
		}
		else
			fThrow = true;
	}

	if (fThrow)
	{
		fprintf(stderr, "[Storage] More pointers were released than originally existed!\n");
		assert(false);
	}
}