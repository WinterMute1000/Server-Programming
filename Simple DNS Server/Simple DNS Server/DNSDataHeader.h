#pragma once
#ifndef _DNSSERVER_LIB_H__
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
typedef struct data_header
{
	short length;
	BOOL is_domain_error;
} DNSDataHeader; //������ ����� ����ü
#endif