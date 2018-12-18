//원본 소스 코드 http://mwultong.blogspot.com/2007/01/c-crc32-crc32cpp.html 
//파일을 모두 전송 후 CRC검사를 통해 파일이 정확히 전송 되었는지 확인 
#pragma once
#ifndef _CRC_H__
#include <stdio.h>
#define DOWNLOAD_ERROR "File Download Error!\n"
#define DOWNLOAD_COMPLETE "FIle Download Complete!\n"
//#define GIGABYTE 1073741824
unsigned long getFileCRC(FILE *); //파일의 CRC값을 얻음
unsigned long calcCRC(const unsigned char *, signed long, unsigned long, unsigned long *); //파일의 CRC값 계산시 사용되는 함수
void makeCRCtable(unsigned long *, unsigned long); //CRC 테이블 만들기
#endif