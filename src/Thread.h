#pragma once

#include <thread>
#include <atomic>
#include <functional>

namespace small_http
{

	class Thread
	{

	public:
		Thread(const char* name);
		virtual ~Thread();

		virtual void TickOnce() = 0;

		void ThreadFunction();

		bool Stop();
		bool Run();

		bool IsRun() { return m_Run; }

		const char* GetName();

	private:
		bool m_Run;
		std::thread* m_ThreadHandle;
		std::string m_Name;
	};

	inline void JoinThread(std::thread* th)
	{
		if (th)
		{
			if (th->joinable())
			{
				th->join();
			}
			delete th;
		}
	}

	inline void sleep(std::uint32_t ms)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));

	}

}
