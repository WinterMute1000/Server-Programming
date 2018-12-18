#pragma once
#pragma warning(disable:4996)
#pragma pack(1)
#include "ServerLib.h"

extern HANDLE thread_pool[THREAD_NUMBER];
extern CRITICAL_SECTION cs;
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

void CreateAndBindSocket(ServerInfoPointer sock_info)
{
	int retval;

	sock_info->sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (sock_info->sock == INVALID_SOCKET)
		err_quit("socket()");

	sock_info->sock_addr.sin_family = AF_INET;
	sock_info->sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	sock_info->sock_addr.sin_port = htons(SERVERPORT);
	retval = bind(sock_info->sock, (SOCKADDR *)&(sock_info->sock_addr), sizeof(sock_info->sock_addr));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");
}

void CreateAndConnectSocket(ClientInfoPointer sock_info)
{
	int retval;

	//sock_info->sock = socket(AF_INET, SOCK_STREAM, 0);
	sock_info->sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (sock_info->sock == INVALID_SOCKET)
	{
		err_quit("socket()");
	}

	sock_info->sock_addr.sin_family = AF_INET;
	sock_info->sock_addr.sin_addr.s_addr = inet_addr(SERVERIP);
	sock_info->sock_addr.sin_port = htons(SERVERPORT);
	retval = connect(sock_info->sock, (SOCKADDR *)&(sock_info->sock_addr), sizeof(sock_info->sock_addr));
	if (retval == SOCKET_ERROR)
		err_quit("connect()");
}

int GetFileSizeToFilePointer(FILE* fp)
{
	int size;

	fseek(fp, 0, SEEK_END);    // 파일 포인터를 파일의 끝으로 이동시킴
	size = ftell(fp);          // 파일 포인터의 현재 위치를 얻음

	fseek(fp, 0, SEEK_SET);

	return size;
}
DWORD WINAPI SendFileData(LPVOID info)
{
	IOInfoPointer io_info = (IOInfoPointer)info;
	HandleInfo handle_info;
	unsigned long send_data = io_info->client_info->file_info.file_size/5;
	int retval, count,bytetrans,err_confirm;
	OVERLAPPED overlapped;
	LPOVERLAPPED overlapped_pointer;
	
	ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	overlapped_pointer = &overlapped;

	EnterCriticalSection(&cs);
	if (send_data >io_info->client_info->file_info.file_size
		- io_info->client_info->file_info.is_file_send) //데이터가 file_size/5 이하로 남았을 시
		send_data = io_info->client_info->file_info.file_size - io_info->client_info->file_info.is_file_send;
	
	count = fread_s((void*)(io_info->client_info->file_info.buffer), send_data,
		1, send_data, io_info->client_info->file_info.file);
	WSASend(io_info->client_info->sock, &(io_info->wsa_buf), 1, &retval, 0, &overlapped, NULL);
	GetQueuedCompletionStatus(io_info->io_port, &bytetrans, (LPDWORD)&handle_info, &overlapped_pointer, INFINITE);
	io_info->client_info->file_info.is_file_send += send_data;
	printf("retval: %zd, 2. count: %zd send data: %d\n", retval, count, io_info->client_info->file_info.is_file_send);

	LeaveCriticalSection(&cs);
	return 0;
}