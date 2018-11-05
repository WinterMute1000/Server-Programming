<<<<<<< HEAD
#pragma comment(lib, "ws2_32")
#pragma warning (disable: 4996)
#include "DNSServer_lib.h"
#include "DNSDataHeader.h"

#define SERVERPORT 9000
#define BUFSIZE    254 //도메인네임의 최대길이+마지막 문자열"\0"만큼 설정

int main(int argc, char *argv[])
{
	int retval;

	// 윈속 초기화
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

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE+1];
	int len;
	DNSDataHeader header;//헤더 데이터 변수
	struct hostent* domain_ip; //도메인 IP정보를 저장할 변수

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

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP Server] Client: IP Address=%s, port=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 클라이언트와 데이터 통신
		while(1)
		{
			// 도메인의 길이를 읽어옴
			retval = RecvDomainLength(client_sock, (char*)&len);
			if(retval == SOCKET_ERROR)
			{
				err_display("Can't receive domain length!");
				break;
			}
			else if(retval == 0)
				break;

			//도메인을 읽어옴
			retval = GetDomain(client_sock, buf, len);
			if (retval == SOCKET_ERROR)
			{
				err_display("Can't receive domain!");
				break;
			}
			else if (retval == 0)
				break;

			// 받은 도메인 출력
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
				ntohs(clientaddr.sin_port), buf);
			//도메인을 IP로 전환
			domain_ip = DomainToIP(buf);

			if (domain_ip == NULL) //도메인 전환 실패시
			{
				printf(ERROR_MESSAGE);
				header.is_domain_error = TRUE;
				header.length = (short)strlen(ERROR_MESSAGE);
			}

			else //성공시
			{
				header.is_domain_error = FALSE;
				header.length = domain_ip->h_length;
				printf("%s %zd", inet_ntoa(*(struct in_addr*)domain_ip->h_addr),
					(size_t)domain_ip->h_length); //보내기전에 확인

			}
			retval = SendHeader(client_sock, &header); //헤더 먼저 보냄
			if (retval == SOCKET_ERROR)
			{
				err_display("Can't Send Header!");
				break;
			}

			retval = 0;
			if(!header.is_domain_error) //에러 아닐 시
				retval = SendIPInfo(client_sock, domain_ip->h_addr, header.length);//변환된 IP를 클라이언트에게 보냄
				//이 도메인을 대표하는 IP주소가 domain_ip->h_addr]이므로 이것을 보냄

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

	// 윈속 종료
	WSACleanup();
	return 0;
}
