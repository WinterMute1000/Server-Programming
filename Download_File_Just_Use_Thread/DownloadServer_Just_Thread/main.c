#include "CRC.h"
#include "SocketLib.h"
#include <process.h>
#pragma comment(lib, "ws2_32")
#pragma warning (disable :4996)

HANDLE thread_pool[THREAD_NUMBER];
int main()
{
	//1. TCP ���� ���� ó�� �� IOCP ���� �ʱ�ȭ(�ʿ��ϴٸ� ����ȭ ó����)

	int retval;
	ServerInfo server_info;
	char *buffer=NULL;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	ZeroMemory(&server_info, sizeof(server_info));
	CreateAndBindSocket(&server_info);

	// listen()
	retval = listen(server_info.sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	// ������ ��ſ� ����� ����
	ClientInfo client_info;
	int addrlen;
	ZeroMemory(&client_info, sizeof(client_info));

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
			client_info.file_info.buffer = (char*)malloc(client_info.file_info.file_size); //��ũ�� ������
		else
			client_info.file_info.buffer = (char*)malloc(GIGABYTE);

		InitializeCriticalSection(&(client_info.cs));

		retval = send(client_info.sock, (char*)&(client_info.file_info.file_size), sizeof(DWORD), 0); //������ ���� ����
		if(retval==SOCKET_ERROR)
		{
			err_display("Send.");
			_getch();
		}

		memset((void*)(client_info.file_info.buffer), 0, client_info.file_info.file_size);

		for (int i = 0; i < THREAD_NUMBER; i++)
			thread_pool[i] = CreateThread(NULL, 0, SendFileData, (void*)&client_info, 0, NULL);

		WaitForMultipleObjects(THREAD_NUMBER, thread_pool, TRUE, INFINITE);
		DeleteCriticalSection(&(client_info.cs)); //������ ���� �� �Ӱ迵�� ����

												   //3. ���� ��� �� CRC�˻�(�������� ó�� �� Ŭ���̾�Ʈ�� ����)
		printf("Download Over. Check CRC......\n");
		unsigned long client_crc, file_crc;
		retval = recv(client_info.sock, (char*)&client_crc, sizeof(unsigned long), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv.");
			_getch();
		}
		fseek(client_info.file_info.file, 0, SEEK_SET);
		file_crc = getFileCRC(client_info.file_info.file);


		printf("Server File CRC: %x         Client File CRC: %x\n", file_crc, client_crc);

		if (file_crc != client_crc) //CRC�� Ʋ���� ������
			printf("Download Fail.\n");
		else  //������ ������
			printf("Download Success.\n");

		CloseHandle((HANDLE)client_info.sock);
		fclose(client_info.file_info.file);
		free(buffer);
	}
	CloseHandle((HANDLE)server_info.sock);
}