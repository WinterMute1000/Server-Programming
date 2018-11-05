#pragma once
#ifndef _DNSSERVER_LIB_H__
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include "DNSDataHeader.h"
#define ERROR_MESSAGE "Can't get Domain\n"
void err_quit(char *msg);
void err_display(char *msg);
int SendIPInfo(SOCKET, char*, short);//클라이언트에게 IP주소 정보 전송
int recvn(SOCKET, char *, int, int);// 사용자 정의 데이터 수신 함수
int RecvDomainLength(SOCKET,char*); //클라이언트가 보낸 도메인의 주소의 길이를 읽어옴
int SendHeader(SOCKET, DNSDataHeader*); //클라이언트에게 헤더를 보냄(데이터 보내기 동일성 및 에러처리)
int GetDomain(SOCKET,char*,int);// 클라이언트가 보낸 도메인을 읽어옴
struct hostent* DomainToIP(const char*); //도메인주소를 IP주소로 전환
#endif 

