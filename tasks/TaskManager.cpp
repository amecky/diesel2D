#include "TaskManager.h"
#include "..\utils\Log.h"

namespace ds {

	TaskThread::TaskThread() {
		m_hWorkEvent[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_hWorkEvent[1] = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_Active = false;
		m_Created = false;
	}

	void TaskThread::Run() {
		LOG << "RUN RUN RUN RUN";
		m_Active = false;
		ResetEvent(m_hWorkEvent[0]);
	}

	DWORD WINAPI TaskThread::ThreadProc(LPVOID Param) 	{
		// Create object of ThreadPoolMgr
		TaskThread* ptrThread = (TaskThread*)Param;

		BOOL bShutDown = FALSE;

		while (!bShutDown) {
			DWORD dwWaitResult = WaitForMultipleObjects(2, ptrThread->m_hWorkEvent, FALSE, INFINITE);
			switch (dwWaitResult) {
			case WAIT_OBJECT_0:
				// Work event signaled Call the Run function    
				ptrThread->Run();
				break;

			case WAIT_OBJECT_0 + 1:
				bShutDown = TRUE;
				break;
			}
		}

		return 0;
	}

	void TaskThread::CreateWorkerThread() {
		m_hThread = CreateThread(NULL, NULL, TaskThread::ThreadProc, (LPVOID)this, NULL, &m_ThreadID);

		if (m_hThread == NULL)
			LOG << "Thread could not be created: " << GetLastError();
		else
			LOG << "Successfully created thread: " << m_ThreadID;
		m_Created = true;
	}

	TaskManager::TaskManager() {
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);

		int numCPU = sysinfo.dwNumberOfProcessors;
		LOG << "Number of cpus: " << numCPU;

		for (int i = 0; i < numCPU - 1; ++i) {
			TaskThread& thread = m_Threads[i];
			thread.CreateWorkerThread();
		}
		numThreads = numCPU - 1;
	}


	TaskManager::~TaskManager() {
	}

	void TaskManager::assign(Task* t) {
		int Count = findFreeThread();
		if (Count != -1)  {
			m_Threads[Count].SetThreadBusy();

			// Set information to thread member so that thread can use it
			m_Threads[Count].setTask(t);

			// Signal Work event
			m_Threads[Count].SignalWorkEvent();
		}
	}

	int TaskManager::findFreeThread() {
		for (int i = 0; i < 32; ++i) {
			if (m_Threads[i].isFree()) {
				return i;
			}
		}
		return -1;
	}

}