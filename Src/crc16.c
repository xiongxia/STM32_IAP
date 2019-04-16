#include "crc16.h"


/**
  * 函数功能: 8位数据高低位颠倒
  * 输入参数: DesBuf 目标字段，SrcBuf 源字段
  * 返 回 值: 无
  * 说    明：无
  */
void InvertUint8(unsigned char *DesBuf, unsigned char *SrcBuf)
{
    int i;
    unsigned char temp = 0;
    
    for(i = 0; i < 8; i++)
    {
        if(SrcBuf[0] & (1 << i))
        {
            temp |= 1<<(7-i);
        }
    }
    DesBuf[0] = temp;
}


/**
  * 函数功能: 16位数据高低位颠倒
  * 输入参数: DesBuf 目标字段，SrcBuf 源字段
  * 返 回 值: 无
  * 说    明：无
  */
void InvertUint16(unsigned short *DesBuf, unsigned short *SrcBuf)  
{  
    int i;  
    unsigned short temp = 0;    
    
    for(i = 0; i < 16; i++)  
    {  
        if(SrcBuf[0] & (1 << i))
        {          
            temp |= 1<<(15 - i);  
        }
    }  
    DesBuf[0] = temp;  
}

/**
  * 函数功能: CRC验证
  * 输入参数: puchMsg 待检测字符串，usDataLen 待检测字符串长度
  * 返 回 值: CRC验证结果,返回0验证通过
  * 说    明：无
  */

unsigned short CRC16_MODBUS(unsigned char *puchMsg, unsigned int usDataLen)  
{  
    unsigned short wCRCin = 0xFFFF;  
    unsigned short wCPoly = 0x8005;  
    unsigned char wChar = 0;  
    
    while (usDataLen--)     
    {  
        wChar = *(puchMsg++);  
        InvertUint8(&wChar, &wChar);  
        wCRCin ^= (wChar << 8); 
        
        for(int i = 0; i < 8; i++)  
        {  
            if(wCRCin & 0x8000) 
            {
                wCRCin = (wCRCin << 1) ^ wCPoly;  
            }
            else  
            {
                wCRCin = wCRCin << 1; 
            }            
        }  
    }  
    InvertUint16(&wCRCin, &wCRCin);  
   // printf("%d,%x",wCRCin,wCRCin);
    return wCRCin ;  
} 






