#include "CRC.h"
#include "SocketLib.h"
#include <process.h>
#pragma comment(lib, "ws2_32")
#pragma warning (disable :4996)
#pragma pack(1)
HANDLE thread_pool[THREAD_NUMBER];
int main(int argc, char *argv[])
{
	ClientInfo sock_info;
	HANDLE set_thread;
	int retval;
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	ZeroMemory(&sock_info, sizeof(ClientInfo));

	CreateAndConnectSocket(&sock_info);
	InitializeCriticalSection(&(sock_info.cs));

	set_thread = CreateThread(NULL, 0, StartRecv, (LPVOID)&sock_info, 0, NULL); //전송 받기 시작

	WaitForSingleObject(set_thread, INFINITE);
	for (int i = 0; i < THREAD_NUMBER; i++)
		thread_pool[i] = CreateThread(NULL, 0, RecvFileData, (LPVOID)&sock_info, 0, NULL); //쓰레드 생성
	WaitForMultipleObjects(THREAD_NUMBER, thread_pool, TRUE, INFINITE);

	//파일을 다 받은 후 CRC 계산
	unsigned long file_crc;

	fclose(sock_info.file_info.file);
	sock_info.file_info.file = fopen("download.mp4", "rb");
	fseek(sock_info.file_info.file, 0, SEEK_SET);
	file_crc = getFileCRC(sock_info.file_info.file);
	printf("CRC Result: %x\n", file_crc);

	retval = send(sock_info.sock, &file_crc, sizeof(unsigned long), 0); //CRC를 보내서 서버로 부터 확인

	if (retval == SOCKET_ERROR)
	{
		printf("Can't Send CRC!");
		exit(-1);
	}
	DeleteCriticalSection(&(sock_info.cs));
	WSACleanup();
	printf("Download Over. Press Any Key.\n");
	_getch();

	fclose(sock_info.file_info.file);
	CloseHandle((HANDLE)sock_info.sock);
	return 0;
}