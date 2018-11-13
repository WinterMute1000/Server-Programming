#pragma once
#ifndef _CHATTING_COMMON_LIB_H__
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#define SERVERPORT 9000
#define BUFSIZE 1024   
#define SERVERIP   "127.0.0.1"
#define END_MESSAGE "END" //ä�� ����� END�� ������ ����ó��
typedef struct socket_info //�����忡 ����ϱ� ���� ���� ���� ����ü
{
	SOCKET sock; //���� ��ũ����
	SOCKADDR_IN sock_addr; //bind()�� connect()�� ���� �ּ� ������ ���� ����ü ����
	char recv_buf[BUFSIZE]; //���ſ� ����
	char send_buf[BUFSIZE]; //�۽ſ� ����
}SocketInfo;
// ���� �Լ� ���� ��� �� ����
void err_quit(char *);
// ���� �Լ� ���� ���
void err_display(char *);
// ����� ���� ������ ���� �Լ�
int recvn(SOCKET, char*, int, int);
void CreateAndBindSocket(SocketInfo*);//socket()+bind() ����� �ϴ� �Լ�
void CreateAndConnectSocket(SocketInfo*); //socket()+connect() ����� �ϴ� �Լ�
int SendChttingOverMessage(SOCKET); //ä�� ���� �޼��� ������ �Լ�
int SendChattingMessage(SocketInfo); //�Ϲ����� �޼����� ������ �Լ�
DWORD WINAPI RecvThreadFunction(LPVOID); //���ſ� ��� �� �����忡 ��� �� �Լ�
#endif
