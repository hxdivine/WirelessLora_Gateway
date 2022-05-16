/******************************************************************************
* Copyright (C) 2017, Huada Semiconductor Co.,Ltd All rights reserved.
*
* This software is owned and published by:
* Huada Semiconductor Co.,Ltd ("HDSC").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with HDSC
* components. This software is licensed by HDSC to be adapted only
* for use in systems utilizing HDSC components. HDSC shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. HDSC is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* Disclaimer:
* HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS),
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
* WARRANTY OF NONINFRINGEMENT.
* HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
* SAVINGS OR PROFITS,
* EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
* FROM, THE SOFTWARE.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/
//Q群：164973950
//TEL：024-85718900
/******************************************************************************/
//Q群：164973950
//TEL：024-85718900
/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2018-04-18  1.0  cj First version for Device Driver Library of Module.
 **
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900

/******************************************************************************
 * Include files
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
#include "ddl.h"
#include "lpuart.h"
#include "lpm.h"
#include "gpio.h"
#include "sysctrl.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900

/******************************************************************************
 * Local type definitions ('typedef')                                         
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                             
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
uint8_t u8TxData[2] = {0x00,0x55};
uint8_t u8RxData[10] = {0x00};
uint8_t u8TxCnt=0,u8RxCnt=0;
uint8_t u8RecvOk = 0;
//void TxIntCallback(void)
//{ 
//	if(u8TxCnt<10)
//	{
//		M0P_LPUART1->SBUF = u8RxData[u8TxCnt++]+0x11;
//	}
//	else
//	{
//		u8TxCnt = 0;
//		u8SendOk = 1;
//		LPUart_ClrStatus(LPUART1,LPUartTC);   
//		LPUart_DisableIrq(LPUART1,LPUartTxIrq);
//		LPUart_EnableIrq(LPUART1,LPUartRxIrq);		
//	}
//}
void RxIntCallback(void)
{
    u8RxData[u8RxCnt]=LPUart_ReceiveData(LPUART1);
    u8RxCnt++;
	if(u8RxCnt>=10)
	{
		LPUart_DisableIrq(LPUART1,LPUartRxIrq);
//		LPUart_EnableIrq(LPUART1,LPUartTxIrq);
		//u8RxCnt = 0;
		u8RecvOk =1;
//		u8TxCnt = 0;
//		M0P_LPUART1->SBUF = u8RxData[0]+0x11;
//		u8TxCnt++;            
	}
}
void ErrIntCallback(void)
{
  
}
void PErrIntCallBack(void)
{
}
void CtsIntCallBack(void)
{
}
//uint32_t *u32RCH = (uint32_t *)0x40002044;
int32_t main(void)
{  
    uint16_t u16Scnt = 0;
    stc_gpio_config_t stcGpioCfg;
    stc_lpuart_sclk_sel_t stcSclk;
    stc_lpuart_config_t  stcConfig;
    stc_lpuart_irq_cb_t stcLPUartIrqCb;
    stc_lpuart_multimode_t stcMulti;
    stc_lpuart_baud_t stcBaud;
    stc_lpm_config_t stcLpmCfg;


    DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcLPUartIrqCb);
    DDL_ZERO_STRUCT(stcMulti);
    DDL_ZERO_STRUCT(stcBaud);
    DDL_ZERO_STRUCT(stcLpmCfg);
    DDL_ZERO_STRUCT(stcGpioCfg);
    DDL_ZERO_STRUCT(stcSclk);
//    *u32RCH = 0x6ff3;
    stcLpmCfg.enSLEEPDEEP = SlpDpEnable;

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralLpUart1,TRUE);
    
    stcGpioCfg.enDir =  GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin0,&stcGpioCfg);//TX
    stcGpioCfg.enDir =  GpioDirIn;
    stcGpioCfg.enPuPd = GpioPu;
    Gpio_Init(GpioPortA,GpioPin1,&stcGpioCfg);//RX
    Gpio_Init(GpioPortD,GpioPin4,&stcGpioCfg);//USER按键
    
    Gpio_SetAfMode(GpioPortA,GpioPin0,GpioAf2);
    Gpio_SetAfMode(GpioPortA,GpioPin1,GpioAf2);
    
    Sysctrl_ClkSourceEnable(SysctrlClkRCL,TRUE);
    Sysctrl_SetRCLTrim(SysctrlRclFreq38400); 
    
    stcLPUartIrqCb.pfnRxIrqCb   = RxIntCallback;
    stcLPUartIrqCb.pfnTxIrqCb   = NULL;
    stcLPUartIrqCb.pfnRxFEIrqCb = ErrIntCallback;
    stcLPUartIrqCb.pfnPEIrqCb   = PErrIntCallBack;
    stcLPUartIrqCb.pfnCtsIrqCb  = CtsIntCallBack;
    stcConfig.pstcIrqCb = &stcLPUartIrqCb;
    stcConfig.bTouchNvic = TRUE;
  
    stcConfig.enStopBit = LPUart1bit;
    stcConfig.enRunMode = LPUartMode3;//
    stcSclk.enSclk_Prs = LPUart4Or8Div;
    stcSclk.enSclk_sel = LPUart_Rcl;
    stcConfig.pstcLpuart_clk = &stcSclk;  
    

    stcMulti.enMulti_mode = LPUartNormal;
    stcConfig.pstcMultiMode = &stcMulti;
    
    LPUart_SetMMDOrCk(LPUART1,LPUartEven);
    LPUart_Init(LPUART1, &stcConfig);
    
    LPUart_SetClkDiv(LPUART1,LPUart4Or8Div);
    stcBaud.u32Sclk = LPUart_GetSclk(LPUART1);
    stcBaud.enRunMode = LPUartMode3;
    stcBaud.u32Baud = 9600;
    u16Scnt = LPUart_CalScnt(LPUART1,&stcBaud);
    LPUart_SetBaud(LPUART1,u16Scnt);
    
    LPUart_EnableIrq(LPUART1,LPUartRxIrq);
    LPUart_ClrStatus(LPUART1,LPUartRC);
    LPUart_EnableFunc(LPUART1,LPUartRx);
    
    while(1 == M0P_GPIO->PDIN_f.PD04);
    Lpm_Config(&stcLpmCfg);
    Lpm_GotoLpmMode();
    while(1)
    {
		if(u8RecvOk)
		{
			u8RxCnt =0; 
			u8RecvOk=0;
			for(u8TxCnt=0;u8TxCnt<10;u8TxCnt++)
			{
				LPUart_SendData(LPUART1,u8RxData[u8TxCnt]+0x11);
			}
			LPUart_EnableIrq(LPUART1,LPUartRxIrq);
			Lpm_Config(&stcLpmCfg);
			Lpm_GotoLpmMode();
        }
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900


