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
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	ZeroMemory(&sock_info, sizeof(SocketInfo));

	CreateAndConnectSocket(&sock_info);

	recv_thread = CreateThread(NULL, 0, RecvThreadFunction, &sock_info, 0, NULL);
	printf("If You Want Send Message, Please Enter.(Chatting Over:End)\n");

	while (!is_chatting_over) //1:�� �̸� select�� IOCP�� ���ؼ� �񵿱��� ������� �����ؾ� ������ 1:1�̹Ƿ� �ϳ��� �ݺ������� ��� ó��
	{
		char temp_buf[BUFSIZE]; //���ڿ� ��ȯ�� �ӽù���

		gets_s(sock_info.send_buf, BUFSIZE);
		if (is_chatting_over) //ä���� while�� ���߿� �б� �����忡�� ������ ���
		{
			printf("Chatting was over!\n");
			continue;
		}
		strncpy(temp_buf, sock_info.send_buf, strnlen(sock_info.send_buf, BUFSIZE-1)+1);
		strupr(temp_buf);

		if (!strncmp(temp_buf, END_MESSAGE,
			strnlen(sock_info.send_buf, BUFSIZE))) //���� �޼��� �Է�Ȯ��
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

			if (retval == SOCKET_ERROR)
			{

				if (!is_chatting_over)//ä���� �����µ� ���� �� ���� ����. �̰��� ó��
				{
					err_display("Send()");
					exit(-1);
				}
			}
		}
	}

	WaitForSingleObject(recv_thread, INFINITE);
	// ���� ����
	WSACleanup();
	return 0;
}