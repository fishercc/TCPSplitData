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
	// ��ʼ���ٽ���
   
}


CDataBufferPool::~CDataBufferPool(void)
{
}

 int  CDataBufferPool::GetFreeSize()
{
	return m_nSize-m_nOffset;
}
/*****************************************************************************
Description:������ݵ�����
Input:pBuff[in]-����ӵ����ݣ�nLen[in]-��������ݳ���
Return: �����ǰ������û���㹻�Ŀռ���pBuff�򷵻�FALSE�����򷵻�TRUE��
******************************************************************************/
bool CDataBufferPool::AddBuff( char *pBuff, int nLen )
{
	m_cs.Lock();///�ٽ�����
	if ( nLen < 0 )
	{
		m_cs.Unlock();
		return false;
	}

	if(nLen <= GetFreeSize())///�ж�ʣ��ռ��Ƿ��㹻���nLen��������
	{
		memcpy(m_pBuff + m_nOffset, pBuff, nLen);
		m_nOffset += nLen;
	}
	else///������������ԭ�еĿռ�
	{ 
		char *p = m_pBuff;
		m_nSize += nLen*2;//ÿ������2*nLen
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
Description:��ȡһ�������İ�
Input:Buf[out]-��ȡ�������ݣ�nLen[out]-��ȡ�������ݳ���
Return: 1����ǰ����������һ����ͷ������ 2����ǰ����������һ�����������
******************************************************************************/
int CDataBufferPool::GetFullPacket(char *Buf, int& nLen)
{
    m_cs.Lock();
    if(m_nOffset < m_PacketHeadLen)//��ǰ����������һ����ͷ������
    {
        m_cs.Unlock();
        return 1;
    }
    
    PACKAGE_HEAD *p = (PACKAGE_HEAD *)m_pBuff;
    if((m_nOffset - m_PacketHeadLen) < (int)p->nDataLen)//��ǰ����������һ�����������
    {
        m_cs.Unlock();
        return 2;
    }

    //�жϰ��ĺϷ���
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