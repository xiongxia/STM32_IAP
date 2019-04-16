/**
  ******************************************************************************
  * 文件名程: main.c 
  * 作    者: 
  * 版    本: V1.0
  * 编写日期: 2018-11-22
  * 功    能: 串口IAP功能实现
  ******************************************************************************
*/
#include "stm32f4xx_hal.h"
#include "string.h"
#include "usart/bsp_debug_usart.h"
#include "stmflash/stm_flash.h"
#include "IAP/bsp_iap.h"
#include "crc16.h"
#include "spiflash/bsp_spiflash.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
uint8_t Rxdata = 0;
uint8_t recv_flag = 0;//接收完成更新标志
uint8_t updata_flag = 0;//记录等待更新时间
uint8_t flag = 0; //调试端口控制进入旧版程序标志
uint16_t recv = 0;//记录接收到安卓数据的字节数
uint8_t android_flag = 0;//接收到安卓数据&标志位 0表示没有接收到 1表示接收到，然后开始接收更新
uint8_t FLAG = 1;
uint16_t crcValue = 1;
uint16_t crcValueTrue = 1;
char ErrorInfo[100] = {0};

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 系统时钟配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
 
  __HAL_RCC_PWR_CLK_ENABLE();                                     //使能PWR时钟

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  //设置调压器输出电压级别1

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;      // 外部晶振，8MHz
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;                        //打开HSE 
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                    //打开PLL
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;            //PLL时钟源选择HSE
  RCC_OscInitStruct.PLL.PLLM = 8;                                 //8分频MHz
  RCC_OscInitStruct.PLL.PLLN = 336;                               //336倍频
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;                     //2分频，得到168MHz主时钟
  RCC_OscInitStruct.PLL.PLLQ = 7;                                 //USB/SDIO/随机数产生器等的主PLL分频系数
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;       // 系统时钟：168MHz
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;              // AHB时钟： 168MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;               // APB1时钟：42MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;               // APB2时钟：84MHz
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  HAL_RCC_EnableCSS();                                            // 使能CSS功能，优先使用外部晶振，内部时钟源为备用
  
 	// HAL_RCC_GetHCLKFreq()/1000    1ms中断一次
	// HAL_RCC_GetHCLKFreq()/100000	 10us中断一次
	// HAL_RCC_GetHCLKFreq()/1000000 1us中断一次
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);                // 配置并启动系统滴答定时器
  /* 系统滴答定时器时钟源 */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* 系统滴答定时器中断优先级配置 */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
void Log()
{
    printf("日志信息:%s",ErrorInfo);
    uint8_t log[110] = {0};
    
    strcat(log,"<");
    strcat(log,ErrorInfo);
    strcat(log,">");
           
    HAL_UART_Transmit(&husart_debug,log,strlen((char *)log),1000);
           
    return;
}

void HexStrToByte(const char* source, unsigned char* dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;
    
    for (i = 0; i < sourceLen; i += 2)
    {
        highByte = toupper(source[i]);
        lowByte  = toupper(source[i + 1]);
 
        if (highByte > 0x39)
            highByte -= 0x37;
        else
            highByte -= 0x30;
 
        if (lowByte > 0x39)
            lowByte -= 0x37;
        else
            lowByte -= 0x30;
 
        dest[i / 2] = (highByte << 4) | lowByte;
    }
    return ;
} 

/**
  * 函数功能: 主函数.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
int main(void)
{
    /* 复位所有外设，初始化Flash接口和系统滴答定时器 */
    HAL_Init();
    /* 配置系统时钟 */
    SystemClock_Config();
    /* 初始化串口并配置串口中断优先级 */
    MX_DEBUG_USART_Init();
    MX_SPIFlash_Init();
    MX_USARTx_Init();
    HAL_UART_Receive_IT(&husart_debug,&Rxdata,1);
    HAL_UART_Receive_IT(&husartx,&Rxdata,1);
    printf("系统正在重启！！\n等待60*10s后,没有接收到更新指令则执行旧版APP");
    printf("如果需要更新 APP，请发送 APP 固件（.bin文件）\n");
    recv = strAppBin .usLength;
    
    uint8_t temp_8_t[5] = {0}; 
    unsigned char CrcTemp[2] = {0};
    /*
    android_flag = 1;
    recv_flag = 0;
    strAppBin .usLength = 0;
    */
    
    
    /* 无限循环 */
    while(1)                            
    {	
	       
        //接收到安卓&并且接收完成数据
        if(recv_flag && android_flag)             
        {
            printf("\nAPP 长度：%d字节\n", strAppBin .usLength );				
            printf("开始更新 APP\n");
	    sprintf(ErrorInfo,"BootLoader:app download finish\n");
            Log();
            //验证程序
            crcValue = CRC16_MODBUS(strAppBin .ucDataBuf,strAppBin .usLength);
	    printf("验证结果:%x\n",crcValue);
            sprintf(ErrorInfo,"BootLoader:CRCValue = %x\n",crcValue);
            Log();
            SPI_FLASH_BufferRead(temp_8_t, FLASH_CRC_CODE,10);
            printf ( "CRC验证码:%c%c%c%c\n",temp_8_t[0],temp_8_t[1],temp_8_t[2],temp_8_t[3]);
            temp_8_t[4] = '\0';
            HexStrToByte(temp_8_t,CrcTemp,4);
	    crcValueTrue = CrcTemp[1] + CrcTemp[0] * 256;
	    printf ( "CRC验证码:%x\n",crcValueTrue);
            sprintf(ErrorInfo,"BootLoader:crcValueTrue = %x\n",crcValueTrue);
            Log();
            //if(1)
	    if(crcValueTrue == crcValue)
            {
                //更新FLASH代码  
                printf ( "程序验证成功\n" ); 
		IAP_Write_App_Bin(APP_START_ADDR, strAppBin .ucDataBuf, strAppBin .usLength );
                printf ( "等待1s后开始执行APP\n" );
                printf ( "开始执行 APP\n" ); 
		sprintf(ErrorInfo,"BootLoader:CRC verification success\n");
                Log();
                recv_flag = 0;  
            }
            else
            {
                printf ( "程序验证失败，执行旧版程序\n" );
                sprintf(ErrorInfo,"BootLoader:CRC verification fail\n");
                Log();
                HAL_UART_Transmit(&husart_debug,")",1,1000);
            }
            //执行FLASH APP代码
            IAP_ExecuteApp(APP_START_ADDR);  
        }
        //等待10s之后，没有收到&，启动旧版程序
        else if(android_flag == 0 && updata_flag >= 10)
        {
            flag = 1;
            updata_flag = 0;
        }
        else
        {
            //没有接收到&，并且小于10s，
            if(!android_flag)
            {
                HAL_Delay(1000);
                updata_flag++;
                printf("%d %02x\n",updata_flag,flag);
            }
            //收到&，但是没有接收完成
            else
            {
            }
        }
        if(flag)
        {
            printf("执行旧版APP\n");
            updata_flag = 0;
	    IAP_ExecuteApp(APP_START_ADDR);
        }
        //还在继续接收数据，接收到数据累加起来
        if(recv != strAppBin .usLength && strAppBin .usLength != 0)
        {
            recv = strAppBin .usLength;
        }
        //没有接收到
        else if(strAppBin .usLength == 0)
        {
            recv_flag = 0;
        }
	//接收完成
        else
        {
            HAL_Delay(1000);
            if(recv == strAppBin .usLength)
            {
                recv_flag = 1;
            }
        }
    }  
}

/**
  * 函数功能: 串口接收完成回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  
  //&结尾，表示发送完成
  if(UartHandle->Instance == DEBUG_USARTx)
  {
      strAppBin.ucDataBuf[strAppBin .usLength] = Rxdata;
      strAppBin .usLength ++;
      HAL_UART_Receive_IT(&husart_debug,&Rxdata,1);
     // printf("%02x %d\n",Rxdata,strAppBin .usLength);
      //printf("%c ",Rxdata);
      //更新系统前需要发送&符号，确认，接下来发送的数据为更新程序
      if(Rxdata == '&' && strAppBin .usLength < 10 && FLAG)
      {
          printf("接收到%c，开始接收更新信息\n",Rxdata);
          android_flag = 1;
          recv_flag = 0;
          strAppBin .usLength = 0;
          FLAG = 0;
      }
  }
  //接收到调试端口发送任意字符则进入旧版程序
  if(UartHandle->Instance == USARTx)
  {
      printf("？？？%02x\n",Rxdata);
      flag = 1;
      HAL_UART_Receive_IT(&husartx,&Rxdata,1);
  }
}

