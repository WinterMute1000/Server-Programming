#pragma once
#ifndef _DNSCLIENT_LIB_H__
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
void ClientEnd(SOCKET); //���� �߻� ���� ���α׷��� ������ ���� �Լ�
void err_quit(char *);
void err_display(char *);
int SendDomainLength(SOCKET, int*); //������ ������ ���� �����ϴ� �Լ�
int SendDomain(SOCKET, char*, int); //������ ������ �����ϴ� �Լ�
int RecvIPLength(SOCKET, short*); /*�������� IP���̸� �޾ƿ��� �Լ�(IP ������ ���̰� 
�����̶� ��� ������ ����� ���ϼ��� ���� ���)*/
int RecvIP(SOCKET, char*,short); //�������� IP������ �޾ƿ�
#endif 

