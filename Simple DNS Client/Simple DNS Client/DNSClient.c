#pragma comment(lib, "ws2_32")
#pragma warning(disable: 4996)
#include "DNSClient_lib.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    254 //도메인 네임의 최대 길이+마지막 문자열 "\0"

int main(int argc, char *argv[])
{
	int retval;
	char domain_name[BUFSIZE];
	char temp_buf[BUFSIZE];
	char ip_info[BUFSIZE];
	int send_len;
	DNSDataHeader recv_header; //헤더를 받아올 변수

	ZeroMemory(&recv_header, sizeof(recv_header));
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	printf("Input Domain Name:");
	scanf("%s", domain_name);

	send_len = (int)strlen(domain_name);
	
	if (send_len > BUFSIZE) //사이즈 검사
	{
		printf("Buffer OverFlow.");
		WSACleanup();
		return 0;
	}

	strncpy(temp_buf, domain_name, send_len); //임시 버퍼에 복사

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

	// 서버와 데이터 통신
	// 도메인네임의 길이 보냄
	retval = SendDomainLength(sock, &send_len);
	if (retval == SOCKET_ERROR)
	{
		err_display("Can't send domain length!");
		ClientEnd(sock);
		return 0;
	}

	// 도메인네임 보내기(가변 길이)
	retval = SendDomain(sock, temp_buf, send_len);
	if (retval == SOCKET_ERROR)
	{
		err_display("Can't send domain!");
		ClientEnd(sock);
		return 0;
	}

	//IP주소 길이를 받음
	retval = RecvHeader(sock, &recv_header);

	if (retval == SOCKET_ERROR)
	{
		err_display("Can't receive Header! ");
		ClientEnd(sock);
		return 0;
	}

	if (recv_header.is_domain_error) //도메인 존재 에러처리
	{
		printf("%s not have domain\n",domain_name);
		printf("Press Any Key.\n");
		getch();

		ClientEnd(sock);
	}
	else
	{
		//IP주소 정보를 받음
		retval = RecvIP(sock, ip_info, recv_header.length);

		if (retval == SOCKET_ERROR)
		{
			err_display("Can't receive IP Info! ");
			ClientEnd(sock);
			return 0;
		}

		printf("%s domain's IP: %s\n", domain_name, inet_ntoa(*(struct in_addr*)ip_info));
		printf("Press Any Key.\n");
		getch();

		ClientEnd(sock);
		return 0;
	}
}
