#pragma once
#ifndef _SOCKET_LIB_H__ //소켓 데이터통신 전용 라이브러리
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#define SERVERPORT 9000
#define SERVERIP   "127.0.0.1"
#define END_MESSAGE "END" //채팅 종료시 END를 보내면 종료처리
#define BUFSIZE 209715200 //한 쓰레드당 200mb씩 보냄
#define THREAD_NUMBER 6 //쓰레드 수를 정의하는 함수 1G/200mb는 대략 5배보다 약간 큼. 그렇기에 6개 생성
#define GIGABYTE 1073741824 //1GB 전송 데이터 확인 용
#define DIVIDE_FILE (x) x/5
typedef struct server_info //서버 정보 구조체
{
	SOCKET sock; //소켓 디스크럽터
	SOCKADDR_IN sock_addr; //bind()나 connect()로 소켓 주소 정보를 받을 구조체 변수
}ServerInfo, *ServerInfoPointer, HandleInfo, *HandleInfoPointer;

typedef struct file_trans_info
{
	char *buffer; //파일 전송 버퍼
	FILE* file; //파일
	DWORD file_size;
	int is_file_send; //기가바이트 보냄 체크(서버 소켓 정보에선 쓰지 않음)
}FileTransInfo, *FileTransInfoPointer;
typedef struct client_info //쓰레드에 사용하기 위한 소켓 정보 구조체
{
	OVERLAPPED overlapped;
	SOCKET sock; //소켓 디스크럽터
	SOCKADDR_IN sock_addr; //bind()나 connect()로 소켓 주소 정보를 받을 구조체 변수
	FileTransInfo file_info;
	int file_len; //클라이언트 전용 파일을 읽고 쓰는 길이 
	CRITICAL_SECTION cs; //데이터 접근 동기화용
}ClientInfo, *ClientInfoPointer;
void err_quit(char *);// 소켓 함수 오류 출력
void err_display(char *);// 사용자 정의 데이터 수신 함수
int recvn(SOCKET, char*, int, int);
void CreateAndBindSocket(ServerInfoPointer);//socket()+bind() 기능을 하는 함수
void CreateAndConnectSocket(ClientInfoPointer); //socket()+connect() 기능을 하는 함수
int GetFileSizeToFilePointer(FILE*);
DWORD WINAPI StartRecv(LPVOID);
DWORD WINAPI RecvFileData(LPVOID);
DWORD WINAPI SendFileData(LPVOID);
#endif