#include "StdAfx.h"
#include "DataBufferPool.h"
#include <winbase.h>



CDataBufferPool::CDataBufferPool(void)
{
	m_nSize =2048;
	m_nOffset =0;
	m_PacketHeadLen = sizeof(PACKAGE_HEAD);
	m_pBuff = new char[m_nSize];
	memset(m_pBuff,0,m_nSize);
	// 初始化临界区
   
}


CDataBufferPool::~CDataBufferPool(void)
{
}

 int  CDataBufferPool::GetFreeSize()
{
	return m_nSize-m_nOffset;
}
/*****************************************************************************
Description:添加数据到缓存
Input:pBuff[in]-待添加的数据；nLen[in]-待添加数据长度
Return: 如果当前缓冲区没有足够的空间存放pBuff则返回FALSE；否则返回TRUE。
******************************************************************************/
bool CDataBufferPool::AddBuff( char *pBuff, int nLen )
{
	m_cs.Lock();///临界区锁
	if ( nLen < 0 )
	{
		m_cs.Unlock();
		return false;
	}

	if(nLen <= GetFreeSize())///判断剩余空间是否足够存放nLen长的数据
	{
		memcpy(m_pBuff + m_nOffset, pBuff, nLen);
		m_nOffset += nLen;
	}
	else///若不够则扩充原有的空间
	{ 
		char *p = m_pBuff;
		m_nSize += nLen*2;//每次增长2*nLen
		m_pBuff = new char[m_nSize];

		memcpy(m_pBuff,p,m_nOffset);
		delete []p;
		memcpy(m_pBuff + m_nOffset, pBuff, nLen);
		m_nOffset += nLen;
		m_cs.Unlock();
		return false;
	}
	m_cs.Unlock();
	return true;
}

/*****************************************************************************
Description:获取一个完整的包
Input:Buf[out]-获取到的数据；nLen[out]-获取到的数据长度
Return: 1、当前缓冲区不够一个包头的数据 2、当前缓冲区不够一个包体的数据
******************************************************************************/
int CDataBufferPool::GetFullPacket(char *Buf, int& nLen)
{
    m_cs.Lock();
    if(m_nOffset < m_PacketHeadLen)//当前缓冲区不够一个包头的数据
    {
        m_cs.Unlock();
        return 1;
    }
    
    PACKAGE_HEAD *p = (PACKAGE_HEAD *)m_pBuff;
    if((m_nOffset - m_PacketHeadLen) < (int)p->nDataLen)//当前缓冲区不够一个包体的数据
    {
        m_cs.Unlock();
        return 2;
    }

    //判断包的合法性
    /* int IsIntegrallity = ValidatePackIntegrality(p);
     if( IsIntegrallity != 0 )
     {
      m_cs.Unlock();
      return IsIntegrallity;
     }
    */
    nLen = m_PacketHeadLen+p->nDataLen;
    memcpy( Buf, m_pBuff+m_PacketHeadLen, nLen );
    m_nOffset -= nLen;
    memcpy( m_pBuff, m_pBuff+nLen, m_nOffset );
	nLen -=m_PacketHeadLen;

    m_cs.Unlock();
    return 0;
}