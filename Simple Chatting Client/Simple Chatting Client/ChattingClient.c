#pragma comment(lib, "ws2_32")
#pragma warning(disable: 4996)
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include "ChattingCommon_lib.h"
extern BOOL is_chatting_over;

int main(int argc, char *argv[])
{
	SocketInfo sock_info;
	HANDLE recv_thread;
	int retval;
	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	ZeroMemory(&sock_info, sizeof(SocketInfo));

	CreateAndConnectSocket(&sock_info);

	recv_thread = CreateThread(NULL, 0, RecvThreadFunction, &sock_info, 0, NULL);
	printf("If You Want Send Message, Please Enter.(Chatting Over:End)\n");

	while (!is_chatting_over) //1:다 이면 select나 IOCP를 통해서 비동기적 입출력을 수행해야 하지만 1:1이므로 하나의 반복문으로 출력 처리
	{
		char temp_buf[BUFSIZE]; //문자열 전환용 임시버퍼

		gets_s(sock_info.send_buf, BUFSIZE);
		if (is_chatting_over) //채팅이 while문 도중에 읽기 쓰레드에서 끝냈을 경우
		{
			printf("Chatting was over!\n");
			continue;
		}
		strncpy(temp_buf, sock_info.send_buf, strnlen(sock_info.send_buf, BUFSIZE-1)+1);
		strupr(temp_buf);

		if (!strncmp(temp_buf, END_MESSAGE,
			strnlen(sock_info.send_buf, BUFSIZE))) //종료 메세지 입력확인
		{
			retval = SendChttingOverMessage(sock_info.sock);

			if (retval == SOCKET_ERROR)
			{
				err_display("Send()");
				exit(-1);
			}

			closesocket(sock_info.sock);
			break;
		}
		else
		{
			retval = SendChattingMessage(sock_info);

			if (!is_chatting_over)//채팅이 끝났는데 보낸 걸 수도 있음. 이것을 처리
			{
				retval = SendChttingOverMessage(sock_info.sock);

				if (retval == SOCKET_ERROR)
				{
					err_display("Send()");
					exit(-1);
				}
			}
		}
	}

	WaitForSingleObject(recv_thread, INFINITE);
	// 윈속 종료
	WSACleanup();
	return 0;
}
