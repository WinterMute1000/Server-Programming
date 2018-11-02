#include"DNSServer_lib.h"
#pragma warning (disable: 4996)
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

int RecvDomainLength(SOCKET client_sock,char* len)
{
	return recvn(client_sock, len, sizeof(int), 0);
}

int GetDomain(SOCKET client_sock, char* buf, int len)
{
	return recvn(client_sock, buf, len, 0);
}

struct hostent* DomainToIP(const char* domain_name)
{
	struct hostent* return_hos;
	return_hos=gethostbyname(domain_name);

	if (!return_hos)
	{
		err_display("gethostbyname failed!");
		exit(1);
	}

	return return_hos;
}

int SendIPLength(SOCKET client_sock, short* len)
{
	return send(client_sock, (char*)len, sizeof(short), 0);
}

int SendIPInfo(SOCKET client_sock, char* domain, short len)
{
	return send(client_sock, domain, len, 0);
}

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