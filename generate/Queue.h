/*!
\file Queue.h
\brief		Definitions for Queue
\author		Rainer Heinelt
\details	Provides definitions for Queues, used to buffer data.
*/
#pragma once

#include <mutex>
#include <condition_variable>
#include <utility>

/*!
\class CStorage
\brief		Holds an array of buffers that can be fetched and later released.
\details	Holds an array of buffers that can be fetched and later released.
Triggers can be installed to get information about the number of buffers remaining.
*/
class CStorage
{
protected:
	CStorage(unsigned int uCount, unsigned int uSize);
	~CStorage(void) noexcept;

	void *GetPointer(void);
	void *GetPointer_Try(void);
	void ReleasePointer(void *pData);

private:
	char *m_pRawData;						///< Raw block of memory
	char **m_apStorageData;					///< Buffer segments in CStorage#m_pRawData.
	volatile unsigned int m_uCurrent = 0;	///< Number of buffers in use.
	const unsigned int m_uCount;			///< Total number of buffers available


	std::mutex m_mtxData;					///< Mutex protecting CStorage#m_uCurrent.
	std::condition_variable	m_condWait;		///< Condition that will wait until a buffer is released.
};

/*!
\class CQueue
\brief		FIFO Queue containing a set amount of elements and allows them to be pushed or popped.
*/
template<typename _Up>
class CQueue : private CStorage
{
public:
	CQueue(unsigned int uCount);
	~CQueue(void) noexcept;

	template<typename... _Arguments>
	void Emplace_Back(_Arguments&&... _Args);
	void Push_Back(const _Up &val);
	_Up *Pop_Front_Ptr(void);
	_Up Pop_Front(void);
	void Release(_Up *p);

private:
	_Up **m_apData;							///< Array of size CQueue#m_uCount containing the buffers in the queue.
	const unsigned int m_uCount;			///< Total number of elements in queue
	volatile unsigned int m_uStart = 0;		///< Index of data available for reading
	volatile unsigned int m_uEnd = 0;		///< Index of data currently written to

	std::mutex m_mtxRead;					///< Mutex protecting CQueue#m_uStart, CQueue#m_uEnd and CQueue#m_apData.
	std::condition_variable m_condWait;		///< Condition that gets signaled once an element is inserted into the queue.
};

template<typename _Up>
inline CQueue<_Up>::CQueue(unsigned int uCount) :
	CStorage(uCount, sizeof(_Up)),
	m_uCount(uCount + 1)
{
	m_apData = new _Up *[uCount];
}

template<typename _Up>
inline CQueue<_Up>::~CQueue(void) noexcept
{
	delete[] m_apData;
}

template<typename _Up>
template<typename... _Args>
inline void CQueue<_Up>::Emplace_Back(_Args&&... __args)
{
	_Up *pData = static_cast<_Up *>(GetPointer());
	try
	{
		::new(pData) _Up(std::forward<_Args>(__args)...);
	}
	catch (...)
	{
		ReleasePointer(pData);
		throw;
	}

	{
		std::unique_lock<std::mutex> lock(m_mtxRead);

		m_apData[m_uEnd] = pData;
		m_uEnd = (m_uEnd + 1) % m_uCount;

		//Signal waiting threads
		m_condWait.notify_one();
	}
}

template<typename _Up>
inline void CQueue<_Up>::Push_Back(const _Up &val)
{
	_Up *pData = static_cast<_Up *>(GetPointer());
	::new(pData) _Up(val);

	{
		std::unique_lock<std::mutex> lock(m_mtxRead);

		m_apData[m_uEnd] = pData;
		m_uEnd = (m_uEnd + 1) % m_uCount;

		//Signal waiting threads
		m_condWait.notify_one();
	}
}

template<typename _Up>
inline _Up *CQueue<_Up>::Pop_Front_Ptr(void)
{
	_Up *pCurrent = nullptr;

	//Lock mutex
	{
		std::unique_lock<std::mutex> lock(m_mtxRead);

		while (m_uStart == m_uEnd)
			m_condWait.wait(lock);	//Queue empty, wait for an element to be pushed

		pCurrent = static_cast<_Up *>(m_apData[m_uStart]);
		m_uStart = (m_uStart + 1) % m_uCount;
	}
	return pCurrent;
}

template<typename _Up>
inline _Up CQueue<_Up>::Pop_Front(void)
{
	_Up *pRet = Pop_Front_Ptr();
	_Up ret = *pRet;
	Release(pRet);
	return ret;
}

template<typename _Up>
inline void CQueue<_Up>::Release(_Up *p)
{
	p->~_Up();
	ReleasePointer(p);
}