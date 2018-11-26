#include "ThreadTestLib.h"

int main()
{
	HANDLE add_thread = NULL, sub_thread = NULL;
	int data = DEFAULT_THREAD_DATA;
	HANDLE thread_pool[2] = { add_thread,sub_thread }; //������ ���� ���� ���� ������ ���� Ǯ

	add_thread = CreateThread(NULL, 0, AddData, &data, 0, NULL);
	sub_thread = CreateThread(NULL, 0, SubData, &data, 0, NULL);
	thread_pool[0] = add_thread; thread_pool[1] = sub_thread;
	WaitForMultipleObjects(2, thread_pool, TRUE, INFINITE); //������ ������� ��ٸ�
	printf("Normal Thread: %d\n",data);
	SET_THREAD_DATA_TO_DEFAULT(data); //��Ȯ�� Ȯ���� ���� �����͸� �ٽ� 10000���� ����
	CloseHandle(add_thread);//����� ������ �ڵ� �ݱ�
	CloseHandle(sub_thread);

	add_thread = CreateThread(NULL, 0, AddData, &data, CREATE_SUSPENDED,NULL); //�������·� �����Ŵ
	sub_thread = CreateThread(NULL, 0, SubData, &data, CREATE_SUSPENDED,NULL);
	thread_pool[0] = add_thread; thread_pool[1] = sub_thread;
	ResumeThread(add_thread); //���ϱ� ������ ���� ����
	ResumeThread(sub_thread);
	WaitForMultipleObjects(2, thread_pool, TRUE, INFINITE);
	printf("Add Thread First: %d\n", data);
	SET_THREAD_DATA_TO_DEFAULT(data);
	CloseHandle(add_thread);
	CloseHandle(sub_thread);

	add_thread = CreateThread(NULL, 0, AddData, &data, CREATE_SUSPENDED,NULL);
	sub_thread = CreateThread(NULL, 0, SubData, &data, CREATE_SUSPENDED,NULL);
	thread_pool[0] = add_thread; thread_pool[1] = sub_thread;
	ResumeThread(sub_thread); //���� ������ ���� ����
	ResumeThread(add_thread);
	WaitForMultipleObjects(2, thread_pool, TRUE, INFINITE);
	printf("Sub Thread First: %d\n", data);
	SET_THREAD_DATA_TO_DEFAULT(data);
	CloseHandle(add_thread);
	CloseHandle(sub_thread);

	//���ϱ� �����尡 ���� �켱 ������ ����
	add_thread = CreateThread(NULL, 0, AddData, &data, CREATE_SUSPENDED,NULL);
	sub_thread = CreateThread(NULL, 0, SubData, &data, CREATE_SUSPENDED,NULL);
	ChangeThreadPriority(add_thread, sub_thread);
	ResumeAllThread(thread_pool);
	WaitForMultipleObjects(2, thread_pool, TRUE, INFINITE);
	printf("Add Thread Low Priority: %d\n", data);
	SET_THREAD_DATA_TO_DEFAULT(data);
	CloseHandle(add_thread);
	CloseHandle(sub_thread);

	//���� �����尡 ���� �켱 ������ ����
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