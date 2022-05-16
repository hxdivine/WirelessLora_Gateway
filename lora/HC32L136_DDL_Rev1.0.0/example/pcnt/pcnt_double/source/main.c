/******************************************************************************
* Copyright (C) 2016, Huada Semiconductor Co.,Ltd All rights reserved.
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
 **   - 2016-02-16  1.0  XYZ First version for Device Driver Library of Module.
 **
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900

/******************************************************************************
 * Include files
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
#include "pca.h"
#include "lpm.h"
#include "gpio.h"
#include "pcnt.h"
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

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
static volatile uint32_t u32InitCntData;
static volatile boolean_t bPcntTestFlag = 0; 

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
#define Button_PORT  GpioPortD
#define Button_PIN   GpioPin4

#define LED_PORT  GpioPortD
#define LED_PIN   GpioPin5

#define LED_ON()     {M0P_GPIO->PDOUT_f.PD05 = 1;}   
#define LED_OFF()    {M0P_GPIO->PDOUT_f.PD05 = 0;} 

#define LED_Tog(x)    {M0P_GPIO->PDOUT_f.PD05 = x;}

#define PCNT0_S0()   {M0P_GPIO->PB05_SEL = 6;  M0P_GPIO->PBDIR_f.PB05 = 1;}  
#define PCNT0_S1()   {M0P_GPIO->PB07_SEL = 7;  M0P_GPIO->PBDIR_f.PB07 = 1;}

#define PCNT1_S0()   {M0P_GPIO->PC00_SEL = 2;  M0P_GPIO->PCDIR_f.PC00 = 1;}  
#define PCNT1_S1()   {M0P_GPIO->PC02_SEL = 3;  M0P_GPIO->PCDIR_f.PC02 = 1;}

#define DeepSleep()   {  SCB->SCR = 0x4;  __WFI();}
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
/*******************************************************************************
 * PCNT中断服务程序
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
void PcntInt(void)
{
     PCNT_Run(FALSE);  

    if (TRUE == PCNT_GetStatus(PCNT_S1E))
    {
        PCNT_ClrStatus(PCNT_S1E);
     
    }
    if (TRUE == PCNT_GetStatus(PCNT_S0E))
    {
        PCNT_ClrStatus(PCNT_S0E);
  
    }
    if (TRUE == PCNT_GetStatus(PCNT_BB))
    {
        PCNT_ClrStatus(PCNT_BB);
    
    }
    if (TRUE == PCNT_GetStatus(PCNT_FE))
    {
        PCNT_ClrStatus(PCNT_FE);
			  
        PCNT_Run(TRUE);
    }
    if (TRUE == PCNT_GetStatus(PCNT_DIR))
    {
        PCNT_ClrStatus(PCNT_DIR);

    }
    if (TRUE == PCNT_GetStatus(PCNT_TO))
    {
        PCNT_ClrStatus(PCNT_TO);
    }
    if (TRUE == PCNT_GetStatus(PCNT_OV))
    {
        PCNT_ClrStatus(PCNT_OV);
			  
			  u32InitCntData++;
			
        PCNT_Parameter(1,1);

        PCNT_Run(TRUE);

        bPcntTestFlag  = ~bPcntTestFlag;
			  
			  LED_Tog(bPcntTestFlag);
			
    }
    if (TRUE == PCNT_GetStatus(PCNT_UF))
    {
        PCNT_ClrStatus(PCNT_UF);
    }	
}



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

int32_t main(void)
{
    stc_pcnt_config_t        stcConfig;
	  stc_gpio_config_t        stcButtonPort;

    u32InitCntData = 0;
    
    DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcButtonPort);
    
	  //使能内部低速RCL时钟，时钟设置为32.768KHZ
    Sysctrl_SetRCLTrim(SysctrlRclFreq32768);
	  Sysctrl_ClkSourceEnable(SysctrlClkRCL,TRUE);
	
    //使能PCNT,GPIO外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralPcnt, TRUE);

	  //PD04设置为GPIO<--SW2控制脚
    stcButtonPort.enDrv  = GpioDrvH;
    stcButtonPort.enDir  = GpioDirIn;
    Gpio_Init(Button_PORT, Button_PIN, &stcButtonPort);
    //PD05设置为GPIO LED
    stcButtonPort.enDrv  = GpioDrvH;
    stcButtonPort.enDir  = GpioDirOut;
    Gpio_Init(LED_PORT, LED_PIN, &stcButtonPort);
		LED_OFF();
		
    PCNT0_S0();
    PCNT0_S1();
    //PCNT初始化		
		stcConfig.bS0Sel = S0P_Invert; //极性取反
    stcConfig.bS1Sel = S1P_Invert; //极性取反
		stcConfig.u8Direc = Direct_Add; //累加计数
		stcConfig.u8Clk = CLK_Rcl;
		stcConfig.u8Mode = Double_Mode;  //双路正交计量方式
    stcConfig.bFLTEn = TRUE;
    stcConfig.u8FLTDep = 5; 		
		stcConfig.u8FLTClk = 32;
		stcConfig.bTOEn = TRUE;
		stcConfig.u16TODep = 1000;
		stcConfig.u8IrqStatus = PCNT_OV|PCNT_TO|PCNT_DIR;
		stcConfig.bIrqEn = TRUE;
		stcConfig.pfnIrqCb = PcntInt;
		
    PCNT_Init(&stcConfig);

    PCNT_Parameter(1,1);
		
		while (TRUE == Gpio_GetInputIO(Button_PORT,Button_PIN));
    
		PCNT_Run(TRUE);
		

		
    while (1)
	  {
		   //DeepSleep();
		}
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900


