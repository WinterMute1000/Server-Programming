#include "ThreadTestLib.h"

int main()
{
	HANDLE add_thread = NULL, sub_thread = NULL;
	int data = DEFAULT_THREAD_DATA;
	HANDLE thread_pool[2] = { add_thread,sub_thread }; //쓰레드 시작 중지 동시 실행을 위한 풀

	add_thread = CreateThread(NULL, 0, AddData, &data, 0, NULL);
	sub_thread = CreateThread(NULL, 0, SubData, &data, 0, NULL);
	thread_pool[0] = add_thread; thread_pool[1] = sub_thread;
	WaitForMultipleObjects(2, thread_pool, TRUE, INFINITE); //쓰레드 종료까지 기다림
	printf("Normal Thread: %d\n",data);
	SET_THREAD_DATA_TO_DEFAULT(data); //정확한 확인을 위해 데이터를 다시 10000으로 세팅
	CloseHandle(add_thread);//종료된 쓰레드 핸들 닫기
	CloseHandle(sub_thread);

	add_thread = CreateThread(NULL, 0, AddData, &data, CREATE_SUSPENDED,NULL); //정지상태로 실행시킴
	sub_thread = CreateThread(NULL, 0, SubData, &data, CREATE_SUSPENDED,NULL);
	thread_pool[0] = add_thread; thread_pool[1] = sub_thread;
	ResumeThread(add_thread); //더하기 쓰레드 먼저 실행
	ResumeThread(sub_thread);
	WaitForMultipleObjects(2, thread_pool, TRUE, INFINITE);
	printf("Add Thread First: %d\n", data);
	SET_THREAD_DATA_TO_DEFAULT(data);
	CloseHandle(add_thread);
	CloseHandle(sub_thread);

	add_thread = CreateThread(NULL, 0, AddData, &data, CREATE_SUSPENDED,NULL);
	sub_thread = CreateThread(NULL, 0, SubData, &data, CREATE_SUSPENDED,NULL);
	thread_pool[0] = add_thread; thread_pool[1] = sub_thread;
	ResumeThread(sub_thread); //빼기 쓰레드 먼저 실행
	ResumeThread(add_thread);
	WaitForMultipleObjects(2, thread_pool, TRUE, INFINITE);
	printf("Sub Thread First: %d\n", data);
	SET_THREAD_DATA_TO_DEFAULT(data);
	CloseHandle(add_thread);
	CloseHandle(sub_thread);

	//더하기 쓰레드가 낮은 우선 순위를 가짐
	add_thread = CreateThread(NULL, 0, AddData, &data, CREATE_SUSPENDED,NULL);
	sub_thread = CreateThread(NULL, 0, SubData, &data, CREATE_SUSPENDED,NULL);
	ChangeThreadPriority(add_thread, sub_thread);
	ResumeAllThread(thread_pool);
	WaitForMultipleObjects(2, thread_pool, TRUE, INFINITE);
	printf("Add Thread Low Priority: %d\n", data);
	SET_THREAD_DATA_TO_DEFAULT(data);
	CloseHandle(add_thread);
	CloseHandle(sub_thread);

	//빼기 쓰레드가 낮은 우선 순위를 가짐
	add_thread = CreateThread(NULL, 0, AddData, &data, CREATE_SUSPENDED,0);
	sub_thread = CreateThread(NULL, 0, SubData, &data, CREATE_SUSPENDED,0);
	ChangeThreadPriority(sub_thread, add_thread);
	ResumeAllThread(thread_pool);
	WaitForMultipleObjects(2, thread_pool, TRUE, INFINITE);
	printf("Sub Thread Low Priority: %d\n", data);
	printf("Press Any Key...");
	CloseHandle(add_thread);
	CloseHandle(sub_thread);
	_getch();

	return 0;
}