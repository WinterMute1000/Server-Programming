#pragma once
#ifndef _SOCKET_LIB_H__ //���� ��������� ���� ���̺귯��
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#define SERVERPORT 9000
#define SERVERIP   "127.0.0.1"
#define END_MESSAGE "END" //ä�� ����� END�� ������ ����ó��
#define BUFSIZE 209715200 //�� ������� 200mb�� ����
#define THREAD_NUMBER 6 //������ ���� �����ϴ� �Լ� 1G/200mb�� �뷫 5�躸�� �ణ ŭ. �׷��⿡ 6�� ����
#define GIGABYTE 1073741824 //1GB ���� ������ Ȯ�� ��
#define DIVIDE_FILE (x) x/5
typedef struct server_info //���� ���� ����ü
{
	SOCKET sock; //���� ��ũ����
	SOCKADDR_IN sock_addr; //bind()�� connect()�� ���� �ּ� ������ ���� ����ü ����
}ServerInfo, *ServerInfoPointer, HandleInfo, *HandleInfoPointer;

typedef struct file_trans_info
{
	char *buffer; //���� ���� ����
	FILE* file; //����
	DWORD file_size;
	int is_file_send; //�Ⱑ����Ʈ ���� üũ(���� ���� �������� ���� ����)
}FileTransInfo, *FileTransInfoPointer;
typedef struct client_info //�����忡 ����ϱ� ���� ���� ���� ����ü
{
	OVERLAPPED overlapped;
	SOCKET sock; //���� ��ũ����
	SOCKADDR_IN sock_addr; //bind()�� connect()�� ���� �ּ� ������ ���� ����ü ����
	FileTransInfo file_info;
	int file_len; //Ŭ���̾�Ʈ ���� ������ �а� ���� ���� 
	CRITICAL_SECTION cs; //������ ���� ����ȭ��
}ClientInfo, *ClientInfoPointer;
void err_quit(char *);// ���� �Լ� ���� ���
void err_display(char *);// ����� ���� ������ ���� �Լ�
int recvn(SOCKET, char*, int, int);
void CreateAndBindSocket(ServerInfoPointer);//socket()+bind() ����� �ϴ� �Լ�
void CreateAndConnectSocket(ClientInfoPointer); //socket()+connect() ����� �ϴ� �Լ�
int GetFileSizeToFilePointer(FILE*);
DWORD WINAPI StartRecv(LPVOID);
DWORD WINAPI RecvFileData(LPVOID);
DWORD WINAPI SendFileData(LPVOID);
#endif