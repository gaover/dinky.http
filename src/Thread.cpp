#include "Thread.h"

namespace small_http
{
	Thread::Thread(const char* name)
		: m_Run(false)
		, m_ThreadHandle(nullptr)
		, m_Name(name)
	{

	}

	Thread::~Thread()
	{
		Stop();
	}

	bool Thread::Stop()
	{
		if (!m_ThreadHandle) return false;

		m_Run = false;

		if (m_ThreadHandle->joinable())
		{
			m_ThreadHandle->join();
		}
		delete m_ThreadHandle;
		m_ThreadHandle = nullptr;

		return true;
	}

	bool Thread::Run()
	{
		if (m_ThreadHandle) return false;
		m_Run = true;

		m_ThreadHandle = new std::thread(std::bind(&Thread::ThreadFunction, this));

		return m_ThreadHandle == nullptr;
	}

	void Thread::ThreadFunction()
	{
		TickOnce();
	}

	const char* Thread::GetName()
	{
		return m_Name.c_str();
	}
}
