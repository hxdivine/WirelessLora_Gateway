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
 **   - 2017-05-28 LiuHL    First Version
 **
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900

/******************************************************************************
 * Include files
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
#include "vc.h"
#include "gpio.h"
#include "adc.h"
#include "adt.h"
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

/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
static uint16_t u16Capture;
static uint16_t u16CaptureBuf;
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
boolean_t bPortVal = 0;
void VcIrqCallback(void)
{
    bPortVal = ~bPortVal;
    Gpio_WriteOutputIO(GpioPortD,GpioPin5,bPortVal);
    Vc_ClearIrq(VcChannel0);
}


void Adt4CaptureBCalllback(void)
{
    Adt_GetCaptureValue(AdtTIM4, AdtCHxB, &u16Capture);
    Adt_GetCaptureBuf(AdtTIM4, AdtCHxB, &u16CaptureBuf);
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
void Vc_PortInit(void)
{
    stc_gpio_config_t stcGpioCfg;
    DDL_ZERO_STRUCT(stcGpioCfg);
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin6,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin6,GpioAf5);//VC??????
    Gpio_Init(GpioPortD,GpioPin5,&stcGpioCfg);
    Gpio_SetAnalogMode(GpioPortC,GpioPin0);//????????????
}
int main(void)
{
    stc_vc_channel_config_t stcChannelConfig;
    
    en_adt_unit_t            enAdt;
    uint16_t                 u16Period;
    en_adt_compare_t         enAdtCompareA;
    uint16_t                 u16CompareA;
    stc_adt_basecnt_cfg_t    stcAdtBaseCntCfg;
    stc_adt_CHxX_port_cfg_t  stcAdtTIM4ACfg;
    stc_adt_CHxX_port_cfg_t  stcAdtTIM4BCfg;
    stc_gpio_config_t        stcTIM4APort;
    stc_gpio_config_t        stcTIM4BPort;
    
    DDL_ZERO_STRUCT(stcChannelConfig);//?????????0
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIM4ACfg);
    DDL_ZERO_STRUCT(stcAdtTIM4BCfg);
    DDL_ZERO_STRUCT(stcTIM4APort);
    DDL_ZERO_STRUCT(stcTIM4BPort);
    
    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE))//???GPIO??????
    {
        return 1;
    }
    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralVcLvd, TRUE))//???LVD??????
    {
        return 1;
    } 
    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE))//???adc??????
    {
        return 1;
    } 	
    M0P_BGR->CR_f.BGR_EN = 0x1u;                 //BGR????????????
    M0P_BGR->CR_f.TS_EN = 0x0u;
    delay100us(1);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);  //????????????????????????
    
    //PA08?????????TIM4_CHA
    //Gpio_ClrAnalogMode(GpioPortA, GpioPin8);
    stcTIM4APort.enDir  = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin8, &stcTIM4APort);
    Gpio_SetAfMode(GpioPortA,GpioPin8,GpioAf6);
    
    //PA07?????????TIM4_CHB
    //Gpio_ClrAnalogMode(GpioPortA, GpioPin7);
    stcTIM4BPort.enDir  = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin7, &stcTIM4BPort);
    Gpio_SetAfMode(GpioPortA,GpioPin7,GpioAf7);


    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE)) //ADT??????????????????
    {
        return Error;
    }
    
    enAdt = AdtTIM4;
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div1024;
    Adt_Init(enAdt, &stcAdtBaseCntCfg);                           //ADT????????????????????????????????????
    
    u16Period = 0xC000;
    Adt_SetPeriod(enAdt, u16Period);                              //????????????
    
    enAdtCompareA = AdtCompareA;
    u16CompareA = 0x6000;
    Adt_SetCompareValue(enAdt, enAdtCompareA, u16CompareA);       //??????????????????????????????A??????
    stcAdtTIM4ACfg.enCap = AdtCHxCompareOutput;                   //CHA????????????
    stcAdtTIM4ACfg.bOutEn = TRUE;                                 //????????????
    stcAdtTIM4ACfg.enPerc = AdtCHxPeriodLow;
    stcAdtTIM4ACfg.enCmpc = AdtCHxCompareHigh;
    stcAdtTIM4ACfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4ACfg.enStaOut = AdtCHxPortOutLow;
    Adt_CHxXPortConfig(enAdt, AdtCHxA, &stcAdtTIM4ACfg);          //??????CHA???????????????????????????
    
    stcAdtTIM4BCfg.enCap = AdtCHxCompareInput;                    //CHB??????????????????
    Adt_CHxXPortConfig(enAdt, AdtCHxB, &stcAdtTIM4BCfg);          //??????CHB???????????????????????????
    
    Adt_ConfigHwCaptureB(enAdt, AdtHwTrigCHxBRise);               //????????????B????????????
    
    Adt_ConfigIrq(enAdt, AdtCMBIrq, TRUE, Adt4CaptureBCalllback); //????????????B??????
     
    Adt_StartCount(enAdt);    
    
    
    //????????????IO???
    Vc_PortInit();
    
    stcChannelConfig.enVcChannel = VcChannel1;
    stcChannelConfig.enVcCmpDly  = VcDelay10mv;
    stcChannelConfig.enVcFilterTime = VcFilter28us;
    stcChannelConfig.enVcInPin_P = VcInPCh0;
    stcChannelConfig.enVcInPin_N = AiBg1p2;
    stcChannelConfig.enVcIrqSel = VcIrqHigh;
    stcChannelConfig.enVcOutConfig = VcOutTIM4;
    stcChannelConfig.pfnAnalogCmpCb = VcIrqCallback;
    Vc_EnableFilter(VcChannel1);
    Vc_ChannelInit(VcChannel1,&stcChannelConfig);
    Vc_EnableIrq(VcChannel1);
    Vc_EnableChannel(VcChannel1);
    while (1)
    {

    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900


