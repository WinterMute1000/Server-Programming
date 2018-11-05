#include "DNSClient_lib.h"
#pragma warning (disable: 4996)

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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
void ClientEnd(SOCKET sock)
{
	closesocket(sock);
	WSACleanup();
}

int SendDomainLength(SOCKET sock, int* send_len)
{
	return send(sock, (char *)send_len, sizeof(int), 0);
}

int SendDomain(SOCKET sock, char* buf, int send_len)
{
	return  send(sock, buf, send_len, 0);
}

int RecvHeader(SOCKET sock, DNSDataHeader* recv_len)
{
	return recv(sock, (char*)recv_len, sizeof(DNSDataHeader), 0);
}
int RecvIP(SOCKET sock, char* buf, short recv_len)
{
	return recv(sock, buf, recv_len, 0);
}
