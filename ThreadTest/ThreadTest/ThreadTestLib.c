#include "ThreadTestLib.h"
DWORD WINAPI AddData(LPVOID data)
{
	int* int_data = (int*)data;

	for (int i = 0; i < DEFAULT_THREAD_DATA; i++) //10000만큼 반복
	{
		(*int_data)++;
	}

	return 0;
}
DWORD WINAPI SubData(LPVOID data)
{
	int* int_data = (int*)data;

	for (int i = 0; i <   DEFAULT_THREAD_DATA; i++)
	{
		(*int_data)--;
	}

	return 0;
}
void SuspendAllThread(const HANDLE* thread_pool)
{
	SuspendThread(thread_pool[0]);
	SuspendThread(thread_pool[1]);
}
void ResumeAllThread(const HANDLE* thread_pool)
{
	ResumeThread(thread_pool[0]);
	ResumeThread(thread_pool[1]);
}
void ChangeThreadPriority(const HANDLE thread_low_priority, const HANDLE thread_high_priority)
{
	SetThreadPriority(thread_low_priority, THREAD_PRIORITY_BELOW_NORMAL);
	SetThreadPriority(thread_high_priority, THREAD_PRIORITY_ABOVE_NORMAL);

	//차이를 주기위해 낮은 우선순위는 THREAD_PRIORIY_BELOW_NORMAL, 높은 우선순위는 THREAD_PRIORITY_ABOVE_NORMAL
}