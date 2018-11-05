#pragma once
#ifndef _DNSSERVER_LIB_H__
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
typedef struct data_header
{
	short length;
	BOOL is_domain_error;
} DNSDataHeader; //데이터 헤더용 구조체
#endif