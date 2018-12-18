#include "CRC.h"
#include "ServerLib.h"
#include <process.h>
#pragma comment(lib, "ws2_32")
#pragma warning (disable :4996)

HANDLE thread_pool[THREAD_NUMBER];
CRITICAL_SECTION cs;
int main()
{
	//1. TCP ���� ���� ó�� �� IOCP ���� �ʱ�ȭ(�ʿ��ϴٸ� ����ȭ ó����)

	int retval,i;
	ServerInfo server_info;
	OVERLAPPED overlapped;
	HandleInfo handle_info;
	LPOVERLAPPED overlapped_pointer;
	DWORD bytetrans=0;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	ZeroMemory(&server_info, sizeof(server_info));
	ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	overlapped_pointer = &overlapped;
	CreateAndBindSocket(&server_info);

	// listen()
	retval = listen(server_info.sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	// ������ ��ſ� ����� ����
	ClientInfo client_info;
	IOInfo io_info;
	int addrlen;
	ZeroMemory(&client_info, sizeof(client_info));

	io_info.io_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	while (1)
	{
		// accept()
		addrlen = sizeof(client_info.sock_addr);
		client_info.sock = accept(server_info.sock, (SOCKADDR *)&(client_info.sock_addr), &addrlen);
		if (client_info.sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		CreateIoCompletionPort((HANDLE)client_info.sock, io_info.io_port, (DWORD)&handle_info, 0);
		io_info.client_info = &client_info;

		printf("File Download Start.\n");
		//2. ���� ����� ���� ������ ���� (�� IOCPó��)
		//���⿡ fopen ���
		client_info.file_info.file = fopen("download.mp4", "rb");
		if (client_info.file_info.file == NULL)
		{
			printf("File Open Error\n");
			_getch();
			exit(-1);
		}
		client_info.file_info.file_size = GetFileSizeToFilePointer(client_info.file_info.file); //������ ���� ��ȯ
		if (client_info.file_info.file_size == 0xFFFFFFFF)
		{
			printf("File Size Error\n");
			_getch();
			exit(-1);
		}
		printf("File Size: %d\n", client_info.file_info.file_size);

		if (client_info.file_info.file_size < GIGABYTE)
			client_info.file_info.buffer = (char*)malloc(client_info.file_info.file_size/5); //��ũ�� ������
		else
		{
			client_info.file_info.file_size = GIGABYTE;
			client_info.file_info.buffer = (char*)malloc(GIGABYTE/5);
		}

		io_info.wsa_buf.buf = io_info.client_info->file_info.buffer;
		io_info.wsa_buf.len = client_info.file_info.file_size / 5;

		InitializeCriticalSection(&cs);
		memset((void*)(client_info.file_info.buffer), 0, client_info.file_info.file_size/5);

		retval = send(client_info.sock, (char*)&(client_info.file_info.file_size),sizeof(int),0); //������ ���� ����
		if (retval == SOCKET_ERROR)
		{
			err_display("Send.");
			_getch();
		}
		for (i = 0; i < THREAD_NUMBER; i++)
			thread_pool[i] = CreateThread(NULL, 0, SendFileData, (void*)&io_info, 0, NULL);

		WaitForMultipleObjects(THREAD_NUMBER, thread_pool, TRUE, INFINITE);
		DeleteCriticalSection(&cs); //������ ���� �� �Ӱ迵�� ����

												   //3. ���� ��� �� CRC�˻�(�������� ó�� �� Ŭ���̾�Ʈ�� ����)
		printf("Download Over. Check CRC......\n");
		unsigned long client_crc, file_crc;
		BOOL is_success;

		retval = recv(client_info.sock, (char*)&client_crc,sizeof(unsigned long),0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv.");
			_getch();
		}
		fseek(client_info.file_info.file, 0, SEEK_SET);
		file_crc = getFileCRC(client_info.file_info.file);

		printf("Server File CRC: %x         Client File CRC: %x\n", file_crc, client_crc);

		if (file_crc != client_crc) //CRC�� Ʋ���� ������
		{
			printf("Download Fail.\n");
			is_success = FALSE;
		}
		else  //������ ������
		{
			printf("Download Success.\n");
			is_success = TRUE;
		}

		retval = send(client_info.sock, &is_success, sizeof(BOOL), 0);
		CloseHandle((HANDLE)client_info.sock);
		fclose(client_info.file_info.file);
		free(client_info.file_info.buffer);
	}
	CloseHandle((HANDLE)server_info.sock);
}