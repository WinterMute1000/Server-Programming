//���� �ҽ� �ڵ� http://mwultong.blogspot.com/2007/01/c-crc32-crc32cpp.html 
//������ ��� ���� �� CRC�˻縦 ���� ������ ��Ȯ�� ���� �Ǿ����� Ȯ�� 
#pragma once
#ifndef _CRC_H__
#include <stdio.h>
#define DOWNLOAD_ERROR "File Download Error!\n"
#define DOWNLOAD_COMPLETE "FIle Download Complete!\n"
//#define GIGABYTE 1073741824
unsigned long getFileCRC(FILE *); //������ CRC���� ����
unsigned long calcCRC(const unsigned char *, signed long, unsigned long, unsigned long *); //������ CRC�� ���� ���Ǵ� �Լ�
void makeCRCtable(unsigned long *, unsigned long); //CRC ���̺� �����
#endif