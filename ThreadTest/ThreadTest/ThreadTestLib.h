#pragma once
#ifndef __THREAD_TEST_LIB_H__
#define DEFAULT_THREAD_DATA 10000
#define SET_THREAD_DATA_TO_DEFAULT(x) x=DEFAULT_THREAD_DATA
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
DWORD WINAPI AddData(LPVOID); //+1 ���� ������ �Լ�
DWORD WINAPI SubData(LPVOID); //-1 ���� ������ �Լ�
void SuspendAllThread(const HANDLE*); //�ΰ��� �����带 ��� ���ߴ� �Լ�
void ResumeAllThread(const HANDLE*); //�ΰ��� �����带 ��� ������ϴ� �Լ�
void ChangeThreadPriority(const HANDLE, const HANDLE); //��������� �켱������ �ٲٴ� �Լ�
#endif
