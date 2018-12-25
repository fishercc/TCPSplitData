// TCPNetSplitData.cpp : 定义控制台应用程序的入口点。
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
		printf("当前缓冲区没有足够的空间存放pBuff\n");
	delete[] buf;
	int len = 4000;
	char *OutBuf = new char[len];
	int res = dbp.GetFullPacket(OutBuf,len);
	if (res==1)
	{
		printf("当前缓冲区没有足够的空间存放pBuff\n");
	}
	if (res==2)
	{
	   printf("当前缓冲区不够一个包体的数据\n");
	}

	 dbp.GetFullPacket(OutBuf,len);
	getchar();

	return 0;
}

