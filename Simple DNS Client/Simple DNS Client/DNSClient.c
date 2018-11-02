#pragma comment(lib, "ws2_32")
#pragma warning(disable: 4996)
#include "DNSClient_lib.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    254 //������ ������ �ִ� ����+������ ���ڿ� "\0"

int main(int argc, char *argv[])
{
	int retval;
	char domain_name[BUFSIZE];
	char temp_buf[BUFSIZE];
	char ip_info[BUFSIZE];
	int send_len;
	short recv_len=0;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	printf("Input Domain Name:");
	scanf("%s", domain_name);

	send_len = (int)strlen(domain_name);
	
	if (send_len > BUFSIZE) //������ �˻�
	{
		printf("Buffer OverFlow.");
		WSACleanup();
		return 0;
	}

	strncpy(temp_buf, domain_name, send_len); //�ӽ� ���ۿ� ����

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// ������ ������ ���
	// �����γ����� ���� ����
	retval = SendDomainLength(sock, &send_len);
	if (retval == SOCKET_ERROR)
	{
		err_display("Can't send domain length!");
		ClientEnd(sock);
		return 0;
	}

	// �����γ��� ������(���� ����)
	retval = SendDomain(sock, temp_buf, send_len);
	if (retval == SOCKET_ERROR)
	{
		err_display("Can't send domain!");
		ClientEnd(sock);
		return 0;
	}

	//IP�ּ� ���̸� ����
	retval = RecvIPLength(sock, &recv_len);

	if (retval == SOCKET_ERROR)
	{
		err_display("Can't receive IP Iength! ");
		ClientEnd(sock);
		return 0;
	}
	//IP�ּ� ������ ����
	retval = RecvIP(sock, ip_info, recv_len);

	if (retval == SOCKET_ERROR)
	{
		err_display("Can't receive IP Info! ");
		ClientEnd(sock);
		return 0;
	}

	printf("%s domain's IP: %s\n",domain_name, inet_ntoa(*(struct in_addr*)ip_info));
	printf("Press Any Key.\n");
	getch();

	ClientEnd(sock);
	return 0;
}