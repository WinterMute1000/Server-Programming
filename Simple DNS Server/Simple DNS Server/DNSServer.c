<<<<<<< HEAD
#pragma comment(lib, "ws2_32")
#pragma warning (disable: 4996)
#include "DNSServer_lib.h"
#include "DNSDataHeader.h"

#define SERVERPORT 9000
#define BUFSIZE    254 //�����γ����� �ִ����+������ ���ڿ�"\0"��ŭ ����

int main(int argc, char *argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE+1];
	int len;
	DNSDataHeader header;//��� ������ ����
	struct hostent* domain_ip; //������ IP������ ������ ����

	ZeroMemory(&header, sizeof(header));

	while(1){
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP Server] Client: IP Address=%s, port=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// Ŭ���̾�Ʈ�� ������ ���
		while(1)
		{
			// �������� ���̸� �о��
			retval = RecvDomainLength(client_sock, (char*)&len);
			if(retval == SOCKET_ERROR)
			{
				err_display("Can't receive domain length!");
				break;
			}
			else if(retval == 0)
				break;

			//�������� �о��
			retval = GetDomain(client_sock, buf, len);
			if (retval == SOCKET_ERROR)
			{
				err_display("Can't receive domain!");
				break;
			}
			else if (retval == 0)
				break;

			// ���� ������ ���
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
				ntohs(clientaddr.sin_port), buf);
			//�������� IP�� ��ȯ
			domain_ip = DomainToIP(buf);

			if (domain_ip == NULL) //������ ��ȯ ���н�
			{
				printf(ERROR_MESSAGE);
				header.is_domain_error = TRUE;
				header.length = (short)strlen(ERROR_MESSAGE);
			}

			else //������
			{
				header.is_domain_error = FALSE;
				header.length = domain_ip->h_length;
				printf("%s %zd", inet_ntoa(*(struct in_addr*)domain_ip->h_addr),
					(size_t)domain_ip->h_length); //���������� Ȯ��

			}
			retval = SendHeader(client_sock, &header); //��� ���� ����
			if (retval == SOCKET_ERROR)
			{
				err_display("Can't Send Header!");
				break;
			}

			retval = 0;
			if(!header.is_domain_error) //���� �ƴ� ��
				retval = SendIPInfo(client_sock, domain_ip->h_addr, header.length);//��ȯ�� IP�� Ŭ���̾�Ʈ���� ����
				//�� �������� ��ǥ�ϴ� IP�ּҰ� domain_ip->h_addr]�̹Ƿ� �̰��� ����

			if (retval == SOCKET_ERROR)
			{
				err_display("Can't Send IP Info!");
				break;
			}
			
		}

		// closesocket()
		closesocket(client_sock);
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
=======
#pragma comment(lib, "ws2_32")
#pragma warning (disable: 4996)
#include "DNSServer_lib.h"

#define SERVERPORT 9000
#define BUFSIZE    254 //�����γ����� �ִ����+������ ���ڿ�"\0"��ŭ ����

int main(int argc, char *argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE+1];
	int len;
	struct hostent* domain_ip; //������ IP������ ������ ����

	while(1){
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP Server] Client: IP Address=%s, port=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// Ŭ���̾�Ʈ�� ������ ���
		while(1)
		{
			// �������� ���̸� �о��
			retval = RecvDomainLength(client_sock, (char*)&len);
			if(retval == SOCKET_ERROR)
			{
				err_display("Can't receive domain length!");
				break;
			}
			else if(retval == 0)
				break;

			//�������� �о��
			retval = GetDomain(client_sock, buf, len);
			if(retval == SOCKET_ERROR)
			{
				err_display("Can't receive domain!");
				break;
			}
			else if(retval == 0)
				break;

			// ���� ������ ���
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
				ntohs(clientaddr.sin_port), buf);
			//�������� IP�� ��ȯ
			domain_ip = DomainToIP(buf);
			printf("%s %zd", inet_ntoa(*(struct in_addr*)domain_ip->h_addr), 
				(size_t)domain_ip->h_length); //���������� Ȯ��

			retval = SendIPLength(client_sock, &domain_ip->h_length); //IP�ּ� ���� ���� ����
			if (retval == SOCKET_ERROR) 
			{
				err_display("Can't Send IP Length!");
				break;
			}
			retval = SendIPInfo(client_sock, domain_ip->h_addr, domain_ip->h_length);//��ȯ�� IP�� Ŭ���̾�Ʈ���� ����
			 //�� �������� ��ǥ�ϴ� IP�ּҰ� domain_ip->h_addr]�̹Ƿ� �̰��� ����
			if (retval == SOCKET_ERROR)
			{
				err_display("Can't Send IP Info!");
				break;
			}
			
		}

		// closesocket()
		closesocket(client_sock);
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
>>>>>>> 59d18402c2039f4eb00bac09463320146b8d21d8
}