// TCPNetSplitData.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdio.h>
#include "DataBufferPool.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CDataBufferPool dbp;
	PACKAGE_HEAD packHead;
	packHead.Version = 1;
	packHead.Command = 0x7e;
	packHead.nDataLen = 10;
	char dat[18]={1,2,3,4,5,6,7,8,9,10,1,0x7e,0,3,0,11,22,33};
	int buflen = sizeof(packHead)+18;
	char *buf = new char[buflen];
	
	memcpy(buf,&packHead,packHead.nDataLen);
	memcpy(buf+sizeof(packHead),dat,18);

	if(!dbp.AddBuff(buf,buflen))
		printf("��ǰ������û���㹻�Ŀռ���pBuff\n");
	delete[] buf;
	int len = 4000;
	char *OutBuf = new char[len];
	int res = dbp.GetFullPacket(OutBuf,len);
	if (res==1)
	{
		printf("��ǰ������û���㹻�Ŀռ���pBuff\n");
	}
	if (res==2)
	{
	   printf("��ǰ����������һ�����������\n");
	}

	 dbp.GetFullPacket(OutBuf,len);
	getchar();

	return 0;
}

