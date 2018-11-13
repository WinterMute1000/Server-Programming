#pragma once
#ifndef _CHATTING_COMMON_LIB_H__
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#define SERVERPORT 9000
#define BUFSIZE 1024   
#define SERVERIP   "127.0.0.1"
#define END_MESSAGE "END" //채팅 종료시 END를 보내면 종료처리
typedef struct socket_info //쓰레드에 사용하기 위한 소켓 정보 구조체
{
	SOCKET sock; //소켓 디스크럽터
	SOCKADDR_IN sock_addr; //bind()나 connect()로 소켓 주소 정보를 받을 구조체 변수
	char recv_buf[BUFSIZE]; //수신용 버퍼
	char send_buf[BUFSIZE]; //송신용 버퍼
}SocketInfo;
// 소켓 함수 오류 출력 후 종료
void err_quit(char *);
// 소켓 함수 오류 출력
void err_display(char *);
// 사용자 정의 데이터 수신 함수
int recvn(SOCKET, char*, int, int);
void CreateAndBindSocket(SocketInfo*);//socket()+bind() 기능을 하는 함수
void CreateAndConnectSocket(SocketInfo*); //socket()+connect() 기능을 하는 함수
int SendChttingOverMessage(SOCKET); //채팅 종료 메세지 보내는 함수
int SendChattingMessage(SocketInfo); //일반적인 메세지를 보내는 함수
DWORD WINAPI RecvThreadFunction(LPVOID); //수신에 사용 될 쓰레드에 사용 될 함수
#endif
