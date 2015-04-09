#pragma once
#include "..\dxstdafx.h"
#include "..\utils\Log.h"
#include <vector>

namespace ds {

	class Task {

	public:
		Task() {}
		virtual ~Task() {}
		virtual void execute() = 0;
	private:

	};

	class LogTask : public Task {

	public:
		LogTask(int frame) : Task() , m_Frame(frame) {}
		virtual ~LogTask() {}
		void execute() {
			LOG << "Hello " << m_Frame;
		}
	private:
		int m_Frame;
	};

	class TaskThread {

	public:
		TaskThread();
		~TaskThread() {
			if (m_Created) {
				CloseHandle(m_hThread);
				CloseHandle(m_hWorkEvent[0]);
				CloseHandle(m_hWorkEvent[1]);
			}
		}
		Task* getTask() {
			return m_Task;
		}
		void    Run(); // Function which will get executed by thread
		static DWORD WINAPI ThreadProc(LPVOID Param); // Thread procedure
		bool isFree() {
			return !m_Active;
		}
		void    CreateWorkerThread();
		HANDLE    GetThreadHandle();
		DWORD    GetThreadID();
		void SignalWorkEvent() {
			SetEvent(m_hWorkEvent[0]);
		}
		void    SignalShutDownEvent();
		void    SetThreadBusy() {
			m_Active = true;
		}
		void    ReleaseHandles();
		void setTask(Task* t) {
			m_Task = t;
		}
	private:
		DWORD    m_ThreadID;        // ID of thread
		HANDLE    m_hThread;        // Handle to thread
		bool    m_Active;            // Flag indicates which is free thread
		HANDLE  m_hWorkEvent[2]; // m_hWorkEvent[0] Work Event. m_hWorkEvent[1] ShutDown event
		Task* m_Task;
		bool m_Created;
	};

	class TaskManager {

		typedef std::vector<Task*> Tasks;

	public:
		TaskManager();
		~TaskManager();
		void add(Task* task) {
			m_Queue.push_back(task);
		}
		void assign(Task* t);
	private:
		int findFreeThread();
		TaskThread m_Threads[32];
		int numThreads;
		Tasks m_Queue;
	};

}

