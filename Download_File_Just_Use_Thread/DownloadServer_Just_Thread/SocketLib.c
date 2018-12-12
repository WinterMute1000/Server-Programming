#pragma once
#pragma warning(disable:4996)
#pragma pack(1)
#include "SocketLib.h"

extern HANDLE thread_pool[THREAD_NUMBER];
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

	sock_info->sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
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
	sock_info->sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
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

DWORD WINAPI StartRecv(LPVOID info)
{
	ClientInfoPointer client_info = (ClientInfoPointer)info;
	int retval,buf_size;
	unsigned long size;
	client_info->file_info.file = fopen("download.mp4", "wb");
	retval = recv(client_info->sock, (void*)&size, sizeof(unsigned long), 0); //파일 길이를 받음

	client_info->file_info.file_size = size;

	if (client_info->file_info.file_size < GIGABYTE)
	{
		buf_size = client_info->file_info.file_size / 5;
		client_info->file_info.buffer = (char*)malloc(client_info->file_info.file_size / 5); //매크로 변경고려
	}
	else
	{
		buf_size = GIGABYTE / 5;
		client_info->file_info.buffer = (char*)malloc(GIGABYTE / 5);
	}

	return 0;
	
}
DWORD WINAPI RecvFileData(LPVOID info)
{
	ClientInfoPointer client_info = (ClientInfoPointer)info;
	size_t retval,count=0;
	unsigned long recv_data= client_info->file_info.file_size / 5;

	EnterCriticalSection(&(client_info->cs));

	if (recv_data <
		client_info->file_info.file_size
		- client_info->file_info.is_file_send) //데이터가 file_size/5 이하로 남음
	{
		recv_data = client_info->file_info.file_size / 5;
		retval = recv(client_info->sock, (void*)(client_info->file_info.buffer), recv_data, 0);
		count=fwrite((void*)(client_info->file_info.buffer),
		    1, recv_data, client_info->file_info.file);
		client_info->file_info.is_file_send += recv_data;
		printf("1. retval: %zd, 2. count: %zd send data: %d\n", retval, count, client_info->file_info.is_file_send);
	}
	else
	{
		recv_data = client_info->file_info.file_size - client_info->file_info.is_file_send;
		retval = recv(client_info->sock, (void*)(client_info->file_info.buffer), recv_data, 0);
		count=fwrite((void*)(client_info->file_info.buffer),
			1, recv_data, client_info->file_info.file);
		printf("2. retval: %zd, 2. count: %zd send data: %d\n", retval, count, client_info->file_info.is_file_send);
	}
	LeaveCriticalSection(&(client_info->cs));
	return 0;
}
DWORD WINAPI SendFileData(LPVOID info)
{
	ClientInfoPointer client_info = (ClientInfoPointer)info;
	unsigned long send_data = client_info->file_info.file_size/5;
	int retval, count;
	
	EnterCriticalSection(&(client_info->cs));
	if (send_data <client_info->file_info.file_size
		- client_info->file_info.is_file_send) //데이터가 file_size/5 이하로 남음
	{
		count=fread_s((void*)(client_info->file_info.buffer), send_data,
			1, send_data, client_info->file_info.file);
		retval = send(client_info->sock,(void*)(client_info->file_info.buffer),send_data,0);
		client_info->file_info.is_file_send += send_data;
		printf("1. retval: %zd, 2. count: %zd send data: %d\n", retval,count, client_info->file_info.is_file_send);
	}
	else
	{
		send_data = client_info->file_info.file_size - client_info->file_info.is_file_send;
		count=fread_s((void*)(client_info->file_info.buffer), send_data,
			1, send_data, client_info->file_info.file);
		retval = send(client_info->sock, (void*)(client_info->file_info.buffer), send_data, 0);
		printf("2. retval: %zd, 2. count: %zd send data: %d\n", retval, count, client_info->file_info.is_file_send);
	}
	LeaveCriticalSection(&(client_info->cs));
	return 0;
}