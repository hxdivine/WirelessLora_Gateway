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
//Q??????164973950
//TEL???024-85718900
/******************************************************************************/
//Q??????164973950
//TEL???024-85718900
/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2016-02-16  1.0  XYZ First version for Device Driver Library of Module.
 **
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900

/******************************************************************************
 * Include files
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
#include "ddl.h"
#include "timer3.h"
#include "lpm.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900


/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900


/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900

/*******************************************************************************
 * BT2??????????????????
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
void Tim3Int(void)
{
     //if(TRUE == Tim3_GetIntFlag(Tim3UevIrq))
     //{
     //    
     //   Tim3_ClearIntFlag(Tim3UevIrq);
     //}
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
//Q??????164973950
//TEL???024-85718900

int32_t main(void)
{
    uint16_t                  u16ArrValue;
    uint16_t                  u16CntValue;
    stc_tim3_mode0_config_t   stcTim3BaseCfg;
    stc_gpio_config_t         stcTIM3A0Port;
    stc_gpio_config_t         stcTIM3B0Port;
    
    DDL_ZERO_STRUCT(stcTim3BaseCfg);
    DDL_ZERO_STRUCT(stcTIM3A0Port);
    DDL_ZERO_STRUCT(stcTIM3B0Port);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //GPIO ??????????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralTim3, TRUE); //Timer3??????????????????
    
    //PA08?????????TIM3_CH0A
    //Gpio_ClrAnalogMode(GpioPortA, GpioPin8);
    stcTIM3A0Port.enDir  = GpioDirOut;
    Gpio_Init(GpioPortA, GpioPin8, &stcTIM3A0Port);
    Gpio_SetAfMode(GpioPortA,GpioPin8,GpioAf2);
    
    //PA07?????????TIM3_CH0B
    //Gpio_ClrAnalogMode(GpioPortA, GpioPin7);
    stcTIM3B0Port.enDir  = GpioDirOut;
    Gpio_Init(GpioPortA, GpioPin7, &stcTIM3B0Port);
    Gpio_SetAfMode(GpioPortA,GpioPin7,GpioAf4);
    
    stcTim3BaseCfg.enWorkMode = Tim3WorkMode0;               //???????????????
    stcTim3BaseCfg.enCT       = Tim3Timer;                   //???????????????????????????????????????PCLK
    stcTim3BaseCfg.enPRS      = Tim3PCLKDiv16;               //PCLK/16
    stcTim3BaseCfg.enCntMode  = Tim316bitArrMode;            //????????????16????????????/?????????
    stcTim3BaseCfg.bEnTog     = TRUE;
    stcTim3BaseCfg.bEnGate    = FALSE;
    stcTim3BaseCfg.enGateP    = Tim3GatePositive;
    
    //stcTim3BaseCfg.pfnTim3Cb  = Tim3Int;                   //??????????????????
    
    Tim3_Mode0_Init(&stcTim3BaseCfg);                        //TIM3 ?????????0???????????????
        
    u16ArrValue = 0x9000;
    
    Tim3_M0_ARRSet(u16ArrValue);                             //???????????????
    
    u16CntValue = 0x9000;
    
    Tim3_M0_Cnt16Set(u16CntValue);                           //??????????????????
    
    //Tim3_ClearIntFlag(Tim3UevIrq);                         //???????????????
    
    //EnableNvic(TIM3_IRQn, 3, TRUE);                        //TIM3 ?????????
    
    //Tim3_Mode0_EnableIrq();                                //??????TIM3??????(??????0?????????????????????)    
    
    Tim3_M0_EnTOG_Output(TRUE);                              //TIM3 ??????????????????
    
    Tim3_M0_Run();                                           //TIM3 ?????????
    
    while (1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900


