#pragma comment(lib, "ws2_32")
#pragma warning (disable :4996)
#include "ChattingCommon_lib.h"
extern BOOL is_chatting_over; //쓰레드 종료(채팅 종료를 확인하기 위한)외부에 쓰일 전역변수
int main(int argc, char *argv[])
{
	int retval;
	SocketInfo server_info;
	HANDLE recv_thread = NULL;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	ZeroMemory(&server_info, sizeof(server_info));

	CreateAndBindSocket(&server_info);

	// listen()
	retval = listen(server_info.sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) 
		err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SocketInfo client_info; //읽기와 쓰기 버퍼를 따로 설정했으므로 쓰레드 동기화는 필요없음
	int addrlen;

	while(1)
	{
		// accept()
		addrlen = sizeof(client_info.sock_addr);
		client_info.sock = accept(server_info.sock, (SOCKADDR *)&(client_info.sock_addr), &addrlen);
		if(client_info.sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		printf("\n[Server] Client Infomation : IP=%s, port:%d\n",
		inet_ntoa(client_info.sock_addr.sin_addr), ntohs(client_info.sock_addr.sin_port));

		// 클라이언트와 데이터 통신
		// 읽기를 따로 처리할 쓰레드 생성
		recv_thread = CreateThread(NULL, 0, RecvThreadFunction, &client_info, 0, NULL);

		// 쓰기는 메인함수에서 처리(따로 처리할 필요가 없음)
		printf("If You Want Send Message, Please Enter.(Chatting Over:End)\n");
		is_chatting_over = FALSE;

		while (!is_chatting_over) //1:다 이면 select나 IOCP를 통해서 비동기적 입출력을 수행해야 하지만 1:1이므로 하나의 반복문으로 출력 처리
		{
			char temp_buf[BUFSIZE]; //문자열 전환용 임시버퍼
			
			gets_s(client_info.send_buf,BUFSIZE);
			if (is_chatting_over) //채팅이 while문 도중에 읽기 쓰레드에서 끝냈을 경우
			{
				printf("Chatting was over!\n");
				continue;
			}
			strncpy(temp_buf, client_info.send_buf, strnlen(client_info.send_buf, BUFSIZE-1)+1);
			strupr(temp_buf);

			if (!strncmp(temp_buf, END_MESSAGE,
				strnlen(client_info.send_buf, BUFSIZE))) //종료 메세지 입력확인
			{
				retval = SendChttingOverMessage(client_info.sock);

				if (retval == SOCKET_ERROR)
				{
					err_display("Send()");
					exit(-1);
				}

				closesocket(client_info.sock);
				break;
			}
			else
			{
				retval = SendChattingMessage(client_info);

				if (retval == SOCKET_ERROR)
				{
						if(!is_chatting_over)//채팅이 끝났는데 보낸 걸 수도 있음. 이것을 처리
						{
							retval = SendChattingOverMessage(client_info.sock);
							if (retval == SOCKET_ERROR)
							{
								err_display("Send()");
								exit(-1);
							}
						}
				}
			}
		}

		WaitForSingleObject(recv_thread, INFINITE);
	}

	// closesocket()
	closesocket(server_info.sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}
