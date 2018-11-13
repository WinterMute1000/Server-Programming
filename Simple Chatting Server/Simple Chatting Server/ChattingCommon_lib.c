#pragma once
#pragma warning(disable:4996)
#include "ChattingCommon_lib.h"

BOOL is_chatting_over = FALSE;

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

	while (left > 0) {
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

void CreateAndBindSocket(SocketInfo* sock_info)
{
	int retval;

	sock_info->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_info->sock == INVALID_SOCKET)
	{
		err_quit("socket()");
	}

	sock_info->sock_addr.sin_family = AF_INET;
	sock_info->sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	sock_info->sock_addr.sin_port = htons(SERVERPORT);
	retval = bind(sock_info->sock, (SOCKADDR *)&(sock_info->sock_addr), sizeof(sock_info->sock_addr));
	if (retval == SOCKET_ERROR) 
		err_quit("bind()");
}

void CreateAndConnectSocket(SocketInfo* sock_info)
{
	int retval;

	sock_info->sock = socket(AF_INET, SOCK_STREAM, 0);
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

DWORD WINAPI RecvThreadFunction(LPVOID arg)
{
	SocketInfo* sock_info = (SocketInfo*)arg;
	int retval;
	int len;

	while (1)
	{
		retval = recvn(sock_info->sock, (char*)&len, sizeof(int), 0); //길이를 먼저 받음

		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			is_chatting_over = TRUE;
			break;
		}
		else if (retval == 0)
			return 0;

		retval = recvn(sock_info->sock, sock_info->recv_buf, len, 0); //그 후 데이터를 수신
		
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			is_chatting_over = TRUE;
			break;
		}
		else if (retval == 0)
			return 0;

		printf("Another Object's Message: %s\n",sock_info->recv_buf);
	}

	return 0;
}

int SendChattingMessage(SocketInfo sock_info)
{
	int retval = 0;
	int len = strnlen(sock_info.send_buf, BUFSIZE - 1) + 1;//\0까지 보내기 위해

	retval = send(sock_info.sock, (char*)&len, sizeof(int), 0); //길이를 먼저 보냄

	if (retval == SOCKET_ERROR)
		return retval;

	retval = send(sock_info.sock, sock_info.send_buf, len, 0); //그 후에 데이터를 보냄

	return retval;
}

int SendChttingOverMessage(SOCKET sock)
{
	int retval = 0;
	int len = strnlen(END_MESSAGE, BUFSIZE - 1) + 1;

	retval = send(sock, (char*)&len, sizeof(int), 0); //길이를 먼저 보냄

	if (retval == SOCKET_ERROR)
		return retval;

	retval = send(sock, END_MESSAGE, len, 0); //그 후에 데이터를 보냄

	return retval;
}
