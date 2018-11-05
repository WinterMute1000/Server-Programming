
#pragma once
#ifndef _DNSCLIENT_LIB_H__
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "DNSDataHeader.h"
void ClientEnd(SOCKET); //에러 발생 때도 프로그램을 끝내기 위한 함수
void err_quit(char *);
void err_display(char *);
int SendDomainLength(SOCKET, int*); //서버에 도메인 길이 전송하는 함수
int SendDomain(SOCKET, char*, int); //서버에 도메인 전송하는 함수
int RecvHeader(SOCKET, DNSDataHeader*); //서버에서 헤더를 받아오는 함수
int RecvIP(SOCKET, char*,short); //서버에서 IP정보를 받아옴
#endif 
