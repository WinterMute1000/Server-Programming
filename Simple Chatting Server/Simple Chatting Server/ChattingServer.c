#pragma comment(lib, "ws2_32")
#pragma warning (disable :4996)
#include "ChattingCommon_lib.h"
extern BOOL is_chatting_over; //������ ����(ä�� ���Ḧ Ȯ���ϱ� ����)�ܺο� ���� ��������
int main(int argc, char *argv[])
{
	int retval;
	SocketInfo server_info;
	HANDLE recv_thread = NULL;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	ZeroMemory(&server_info, sizeof(server_info));

	CreateAndBindSocket(&server_info);

	// listen()
	retval = listen(server_info.sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) 
		err_quit("listen()");

	// ������ ��ſ� ����� ����
	SocketInfo client_info; //�б�� ���� ���۸� ���� ���������Ƿ� ������ ����ȭ�� �ʿ����
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

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[Server] Client Infomation : IP=%s, port:%d\n",
		inet_ntoa(client_info.sock_addr.sin_addr), ntohs(client_info.sock_addr.sin_port));

		// Ŭ���̾�Ʈ�� ������ ���
		// �б⸦ ���� ó���� ������ ����
		recv_thread = CreateThread(NULL, 0, RecvThreadFunction, &client_info, 0, NULL);

		// ����� �����Լ����� ó��(���� ó���� �ʿ䰡 ����)
		printf("If You Want Send Message, Please Enter.(Chatting Over:End)\n");
		is_chatting_over = FALSE;

		while (!is_chatting_over) //1:�� �̸� select�� IOCP�� ���ؼ� �񵿱��� ������� �����ؾ� ������ 1:1�̹Ƿ� �ϳ��� �ݺ������� ��� ó��
		{
			char temp_buf[BUFSIZE]; //���ڿ� ��ȯ�� �ӽù���
			
			gets_s(client_info.send_buf,BUFSIZE);
			if (is_chatting_over) //ä���� while�� ���߿� �б� �����忡�� ������ ���
			{
				printf("Chatting was over!\n");
				continue;
			}
			strncpy(temp_buf, client_info.send_buf, strnlen(client_info.send_buf, BUFSIZE-1)+1);
			strupr(temp_buf);

			if (!strncmp(temp_buf, END_MESSAGE,
				strnlen(client_info.send_buf, BUFSIZE))) //���� �޼��� �Է�Ȯ��
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
						if(!is_chatting_over)//ä���� �����µ� ���� �� ���� ����. �̰��� ó��
						{
							err_display("Send()");
							exit(-1);
						}
				}
			}
		}

		WaitForSingleObject(recv_thread, INFINITE);
	}

	// closesocket()
	closesocket(server_info.sock);

	// ���� ����
	WSACleanup();
	return 0;
}