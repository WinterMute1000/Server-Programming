#pragma once
#ifndef __THREAD_TEST_LIB_H__
#define DEFAULT_THREAD_DATA 10000
#define SET_THREAD_DATA_TO_DEFAULT(x) x=DEFAULT_THREAD_DATA
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
DWORD WINAPI AddData(LPVOID); //+1 전용 쓰레드 함수
DWORD WINAPI SubData(LPVOID); //-1 전용 쓰레드 함수
void SuspendAllThread(const HANDLE*); //두개의 쓰레드를 모두 멈추는 함수
void ResumeAllThread(const HANDLE*); //두개의 쓰레드를 모두 재시작하는 함수
void ChangeThreadPriority(const HANDLE, const HANDLE); //쓰레드들의 우선순위를 바꾸는 함수
#endif
