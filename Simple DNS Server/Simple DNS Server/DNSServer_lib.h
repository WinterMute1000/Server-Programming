#pragma once
#ifndef _DNSSERVER_LIB_H__
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
void err_quit(char *msg);
void err_display(char *msg);
int SendIPInfo(SOCKET, char*, short);//Ŭ���̾�Ʈ���� IP�ּ� ���� ����
int recvn(SOCKET, char *, int, int);// ����� ���� ������ ���� �Լ�
int RecvDomainLength(SOCKET,char*); //Ŭ���̾�Ʈ�� ���� �������� �ּ��� ���̸� �о��
int SendIPLength(SOCKET, short*); //Ŭ���̾�Ʈ���� IP�ּ��� ���̸� ����(������ ������ ���ϼ��� ����)
int GetDomain(SOCKET,char*,int);// Ŭ���̾�Ʈ�� ���� �������� �о��
struct hostent* DomainToIP(const char*); //�������ּҸ� IP�ּҷ� ��ȯ
#endif 

