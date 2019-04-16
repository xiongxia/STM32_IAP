/**
  ******************************************************************************
  * 文件名程: bsp_iap.c 
  * 作    者: 
  * 版    本: V1.0
  * 编写日期: 2018-11-22
  * 功    能: IAP底层驱动实现
  ******************************************************************************

/* 包含头文件 ----------------------------------------------------------------*/
#include "IAP/bsp_iap.h"
#include "stmflash/stm_flash.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/

struct STRUCT_IAP_RECIEVE strAppBin;//={{0},0};

static uint16_t ulBuf_Flash_App[1024];

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
void IAP_Write_App_Bin ( uint32_t ulStartAddr, uint8_t * pBin_DataBuf, uint32_t ulBufLength )
{
	uint16_t us, usCtr=0, usTemp;
	uint32_t ulAdd_Write = ulStartAddr;                                //当前写入的地址
	uint8_t * pData = pBin_DataBuf;
	
        
        //printf("APP地址：%d\n",ulStartAddr);
        
	for (us = 0; us < ulBufLength; us += 2)
	{						    
          usTemp =  ( uint16_t ) pData[1]<<8;
          usTemp += ( uint16_t ) pData[0];	  
          pData += 2;                                                      //偏移2个字节
          ulBuf_Flash_App [ usCtr ++ ] = usTemp;	    
          if ( usCtr == 1024 )
          {
            printf("APP写入次数：%d\n",ulAdd_Write);
            usCtr = 0;
            STMFLASH_Write ( ulAdd_Write, ulBuf_Flash_App, 1024 );	
            //STMFLASH_Write_NoCheck(ulAdd_Write, ulBuf_Flash_App, 1024);
	    ulAdd_Write += 2048;                                           //偏移2048  16=2*8.所以要乘以2.
          }
	}
	if (usCtr) 
        { 
          STMFLASH_Write (ulAdd_Write,ulBuf_Flash_App,usCtr );//将最后的一些内容字节写进去. 
          //STMFLASH_Write_NoCheck(ulAdd_Write, ulBuf_Flash_App, usCtr);
        }
}
/*
void Write_App_Bin_To_Flash ( uint32_t ulStartAddr, uint8_t * pBin_DataBuf, uint32_t ulBufLength )
{
	uint16_t us, usCtr=0, usTemp;
	uint32_t ulAdd_Write = ulStartAddr;                                //当前写入的地址
	uint8_t * pData = pBin_DataBuf;
	
        
        printf("APP地址：%d\n",ulStartAddr);
        
	for (us = 0; us < ulBufLength; us += 2)
	{						    
          usTemp =  ( uint16_t ) pData[1]<<8;
          usTemp += ( uint16_t ) pData[0];	  
          pData += 2;                                                      //偏移2个字节
          ulBuf_Flash_App [ usCtr ++ ] = usTemp;	    
          if ( usCtr == 1024 )
          {
            printf("APP写入次数：%d\n",ulAdd_Write);
            usCtr = 0;
            STMFLASH_Write ( ulAdd_Write, ulBuf_Flash_App, 1024 );	
            //STMFLASH_Write_NoCheck(ulAdd_Write, ulBuf_Flash_App, 1024);
	    ulAdd_Write += 2048;                                           //偏移2048  16=2*8.所以要乘以2.
          }
	}
	if (usCtr) 
        { 
          STMFLASH_Write (ulAdd_Write,ulBuf_Flash_App,usCtr );//将最后的一些内容字节写进去. 
          //STMFLASH_Write_NoCheck(ulAdd_Write, ulBuf_Flash_App, usCtr);
        }
}*/

void MSR_MSP ( uint32_t ulAddr ) 
{
    asm("MSR MSP, r0"); 			                   //set Main Stack value
    asm("BX r14");
}

//跳转到应用程序段
//ulAddr_App:用户代码起始地址.
void IAP_ExecuteApp ( uint32_t ulAddr_App )
{
	pIapFun_TypeDef pJump2App; 
	
	if ( ( ( * ( __IO uint32_t * ) ulAddr_App ) & 0x2FF00000 ) == 0x20000000 )	  //检查栈顶地址是否合法.
	{ 
                printf("开始执行\n");
		pJump2App = ( pIapFun_TypeDef ) * ( __IO uint32_t * ) ( ulAddr_App + 4 );	//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP( * ( __IO uint32_t * ) ulAddr_App );			     //初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		pJump2App ();								  	//跳转到APP.
	}
}		



/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
