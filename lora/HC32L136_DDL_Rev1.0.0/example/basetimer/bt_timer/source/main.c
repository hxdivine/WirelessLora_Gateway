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
#include "bt.h"
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
 * BT1??????????????????
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
void Tim0Int(void)
{
    static uint8_t i;
    
    if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
    {
        if(i == 0)
        {
            Gpio_WriteOutputIO(GpioPortD, GpioPin5, TRUE);
            
            i = 1;
        }
        else
        {
            Gpio_WriteOutputIO(GpioPortD, GpioPin5, FALSE);
            
            i = 0;
        }
        
        Bt_ClearIntFlag(TIM0,BtUevIrq);
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
//Q??????164973950
//TEL???024-85718900

int32_t main(void)
{
    uint16_t                  u16ArrValue;
    uint16_t                  u16CntValue;
    stc_bt_mode0_config_t     stcBtBaseCfg;
    stc_gpio_config_t         stcLEDPortCfg;

    
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    DDL_ZERO_STRUCT(stcLEDPortCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //GPIO ??????????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBTim, TRUE); //Base Timer??????????????????
    
    //PD05?????????LED ????????????
    //Gpio_ClrAnalogMode(GpioPortD, GpioPin5);
    stcLEDPortCfg.enDir  = GpioDirOut;
    Gpio_Init(GpioPortD, GpioPin5, &stcLEDPortCfg);
    
    stcBtBaseCfg.enWorkMode = BtWorkMode0;                  //???????????????
    stcBtBaseCfg.enCT       = BtTimer;                      //???????????????????????????????????????PCLK
    stcBtBaseCfg.enPRS      = BtPCLKDiv64;                  //PCLK/64
    stcBtBaseCfg.enCntMode  = Bt16bitArrMode;               //????????????16????????????/?????????
    stcBtBaseCfg.bEnTog     = FALSE;
    stcBtBaseCfg.bEnGate    = FALSE;
    stcBtBaseCfg.enGateP    = BtGatePositive;
    
    stcBtBaseCfg.pfnTim0Cb  = Tim0Int;                      //??????????????????
    
    Bt_Mode0_Init(TIM0, &stcBtBaseCfg);                     //TIM0 ?????????0???????????????
    
    u16ArrValue = 0x6000;
    
    Bt_M0_ARRSet(TIM0, u16ArrValue);                        //???????????????(?????? = 0x10000 - ARR)
    
    u16CntValue = 0x6000;
    
    Bt_M0_Cnt16Set(TIM0, u16CntValue);                      //??????????????????
    
    Bt_ClearIntFlag(TIM0,BtUevIrq);                         //???????????????   
    Bt_Mode0_EnableIrq(TIM0);                               //??????TIM0??????(??????0?????????????????????)
    EnableNvic(TIM0_IRQn, IrqLevel3, TRUE);                 //TIM0????????????
    
    Bt_M0_Run(TIM0);                                        //TIM0 ?????????
    
    while (1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900


