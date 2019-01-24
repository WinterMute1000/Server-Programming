#include "CRC.h"
#include "ServerLib.h"
#include <process.h>
#pragma comment(lib, "ws2_32")
#pragma warning (disable :4996)

HANDLE thread_pool[THREAD_NUMBER];
CRITICAL_SECTION cs;
int main()
{
	//1. TCP 소켓 연결 처리 및 IOCP 관련 초기화(필요하다면 동기화 처리도)

	int retval,i;
	ServerInfo server_info;
	OVERLAPPED overlapped;
	HandleInfo handle_info;
	LPOVERLAPPED overlapped_pointer;
	DWORD bytetrans=0;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	ZeroMemory(&server_info, sizeof(server_info));
	ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	overlapped_pointer = &overlapped;
	CreateAndBindSocket(&server_info);

	// listen()
	retval = listen(server_info.sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	// 데이터 통신에 사용할 변수
	ClientInfo client_info;
	IOInfo io_info;
	int addrlen;
	ZeroMemory(&client_info, sizeof(client_info));

	io_info.io_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	while (1)
	{
		// accept()
		addrlen = sizeof(client_info.sock_addr);
		client_info.sock = accept(server_info.sock, (SOCKADDR *)&(client_info.sock_addr), &addrlen);
		if (client_info.sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		CreateIoCompletionPort((HANDLE)client_info.sock, io_info.io_port, (DWORD)&handle_info, 0);
		io_info.client_info = &client_info;

		printf("File Download Start.\n");
		//2. 파일 출력을 위한 쓰레드 생성 (및 IOCP처리)
		//여기에 fopen 사용
		client_info.file_info.file = fopen("download.mp4", "rb");
		if (client_info.file_info.file == NULL)
		{
			printf("File Open Error\n");
			_getch();
			exit(-1);
		}
		client_info.file_info.file_size = GetFileSizeToFilePointer(client_info.file_info.file); //파일의 길이 변환
		if (client_info.file_info.file_size == 0xFFFFFFFF)
		{
			printf("File Size Error\n");
			_getch();
			exit(-1);
		}
		printf("File Size: %d\n", client_info.file_info.file_size);

		if (client_info.file_info.file_size < GIGABYTE)
			client_info.file_info.buffer = (char*)malloc(client_info.file_info.file_size/5); //매크로 변경고려
		else
		{
			client_info.file_info.file_size = GIGABYTE;
			client_info.file_info.buffer = (char*)malloc(GIGABYTE/5);
		}

		io_info.wsa_buf.buf = client_info.file_info.buffer;
		io_info.wsa_buf.len = client_info.file_info.file_size / 5;

		InitializeCriticalSection(&cs);
		memset((void*)(client_info.file_info.buffer), 0, client_info.file_info.file_size/5);

		retval = send(client_info.sock, (char*)&(client_info.file_info.file_size),sizeof(int),0); //파일의 길이 보냄
		if (retval == SOCKET_ERROR)
		{
			err_display("Send.");
			_getch();
		}
		for (i = 0; i < THREAD_NUMBER; i++)
			thread_pool[i] = CreateThread(NULL, 0, SendFileData, (void*)&io_info, 0, NULL);

		WaitForMultipleObjects(THREAD_NUMBER, thread_pool, TRUE, INFINITE);
		free(client_info.file_info.buffer);
		DeleteCriticalSection(&cs); //쓰레드 종료 후 임계영역 삭제

												   //3. 파일 출력 후 CRC검사(서버에서 처리 후 클라이언트에 보냄)
		printf("Download Over. Check CRC......\n");
		unsigned long client_crc, file_crc;
		BOOL is_success;

		retval = recv(client_info.sock, (char*)&client_crc,sizeof(unsigned long),0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv.");
			_getch();
		}
		fseek(client_info.file_info.file, 0, SEEK_SET);
		file_crc = getFileCRC(client_info.file_info.file);

		printf("Server File CRC: %x         Client File CRC: %x\n", file_crc, client_crc);

		if (file_crc != client_crc) //CRC가 틀리면 실패임
		{
			printf("Download Fail.\n");
			is_success = FALSE;
		}
		else  //같으면 성공임
		{
			printf("Download Success.\n");
			is_success = TRUE;
		}

		retval = send(client_info.sock, &is_success, sizeof(BOOL), 0);
		CloseHandle((HANDLE)client_info.sock);
		fclose(client_info.file_info.file);
		free(client_info.file_info.buffer);
	}
	CloseHandle((HANDLE)server_info.sock);
}
