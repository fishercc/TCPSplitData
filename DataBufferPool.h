/*最初遇到"粘包"的问题时，我是通过在两次 send 之间调用 sleep 来休眠一小段时间来解决。

	这个解决方法的缺点是显而易见的：使传输效率大大降低，而且也并不可靠。
	后来就是通过应答的方式来解决，尽管在大多数时候是可行的，但是不能解决象 B 的那种情况，
	而且采用应答方式增加了通讯量，加重了网络负荷（但是象FTP等协议采用的就是应答方式）。
	再后来就是对数据包进行封包和拆包的操作。

	封包：
	封包就是给一段数据加上包头，这样一来数据包就分为包头和包体两部分内容了（以后讲过滤非法包时封包会加入"包尾"内容）。

	包头其实上是个大小固定的结构体，其中有个结构体成员变量表示包体的长度，
	这是个很重要的变量，其他的结构体成员可根据需要自己定义。
	根据包头长度固定以及包头中含有包体长度的变量就能正确的拆分出一个完整的数据包。

	拆包：
	对于拆包目前我最常用的是以下两种方式：
	1、动态缓冲区暂存方式。之所以说缓冲区是动态的是因为当需要缓冲的数据长度超出缓冲区的长度时会增大缓冲区长度。
	大概过程描述如下：
	A 为每一个连接动态分配一个缓冲区，同时把此缓冲区和 SOCKET 关联，常用的是通过结构体关联。
	B 当接收到数据时首先把此段数据存放在缓冲区中。
	C 判断缓存区中的数据长度是否够一个包头的长度，如不够，则不进行拆包操作。
	D 根据包头数据解析出里面代表包体长度的变量。
	E 判断缓存区中除包头外的数据长度是否够一个包体的长度，如不够，则不进行拆包操作。
	F 取出整个数据包，这里的"取"的意思是不光从缓冲区中拷贝出数据包，而且要把此数据包从缓存区中删除掉。
	  删除的办法就是把此包后面的数据移动到缓冲区的起始地址。

	这种方法有两个缺点：

	1）为每个连接动态分配一个缓冲区增大了内存的使用；

	2）有三个地方需要拷贝数据，一个地方是把数据存放在缓冲区，
	一个地方是把完整的数据包从缓冲区取出来，一个地方是把数据包从缓冲区中删除。
	这种拆包的改进方法会解决和完善部分缺点。
*/



#pragma once

#include <WinSock2.h>//内包含CRITICAL_SECTION
#include <list>
#include <process.h>



#pragma pack(push,1) //开始定义数据包, 采用字节对齐方式
/*----------------------包头---------------------*/
typedef struct tagPACKAGEHEAD
{
	BYTE Version;//版本
	WORD Command;//命令
	WORD nDataLen;//包体的长度
}PACKAGE_HEAD;
#pragma pack(pop) //结束定义数据包, 恢复原来对齐方式






class myCritical_Section{
private:
	// 临界区结构对象
	CRITICAL_SECTION m_cs;

public:
	 myCritical_Section(void){
		 InitializeCriticalSection(&m_cs);
	 }
	


public:
	void Lock(){
		EnterCriticalSection(&m_cs);

	}
	void Unlock(){
		LeaveCriticalSection(&m_cs);
	}
};

class CDataBufferPool
{
private:
	myCritical_Section m_cs;
	char *m_pBuff;//缓存数组指针
	 int m_nSize;//缓存区大小
	 int m_nOffset;//偏移量
	 int m_PacketHeadLen;//包头长度
public:
	CDataBufferPool(void);
	~CDataBufferPool(void);
	//添加数据到缓存
	bool CDataBufferPool::AddBuff( char *pBuff, int nLen );
	//获取一个完整的包
	int CDataBufferPool::GetFullPacket(char *Buf, int& nLen);
	 int  GetFreeSize();
};


