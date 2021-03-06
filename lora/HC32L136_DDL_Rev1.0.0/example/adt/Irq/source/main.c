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
 **   - 2017-06-20 LiuHL    First Version
 **
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900

/******************************************************************************
 * Include files
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
#include "adt.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
#define DEBUG_PRINT

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
static stc_adt_cntstate_cfg_t stcAdt5CntState;

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900

void Adt5CompACalllback(void)
{
    Adt_GetCntState(AdtTIM5, &stcAdt5CntState);
}

void Adt5CompBCalllback(void)
{
    Adt_GetCntState(AdtTIM5, &stcAdt5CntState);
}
void Adt5CompCCalllback(void)
{
    Adt_GetCntState(AdtTIM5, &stcAdt5CntState);
}
void Adt5CompDCalllback(void)
{
    Adt_GetCntState(AdtTIM5, &stcAdt5CntState);
}

void Adt5CaptureACalllback(void)
{
    uint16_t u16Capture;
    
    Adt_GetCaptureValue(AdtTIM5, AdtCHxA, &u16Capture);
    Adt_GetCntState(AdtTIM5, &stcAdt5CntState);
}

void Adt5CaptureBCalllback(void)
{
    uint16_t u16Capture;
    
    Adt_GetCaptureValue(AdtTIM5, AdtCHxB, &u16Capture);
    Adt_GetCntState(AdtTIM5, &stcAdt5CntState);
}

void Adt5OVFCalllback(void)
{
    Adt_GetCntState(AdtTIM5, &stcAdt5CntState);
}
void Adt5UDFCalllback(void)
{
    Adt_GetCntState(AdtTIM5, &stcAdt5CntState);
}

void Adt5DTECalllback(void)
{
    Adt_StopCount(AdtTIM5);
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
    en_adt_unit_t enAdt;
    uint16_t u16Period;
    en_adt_compare_t enAdtCompare;
    uint16_t u16Compare;
    
    stc_adt_basecnt_cfg_t     stcAdtBaseCntCfg;
    stc_adt_CHxX_port_cfg_t   stcAdtTIMACfg;
    stc_adt_CHxX_port_cfg_t   stcAdtTIMBCfg;
    stc_adt_port_trig_cfg_t   stcAdtPortTrig;
    stc_gpio_config_t         stcTIM5APort;
    stc_gpio_config_t         stcTIM5BPort;
    stc_gpio_config_t         stcTRIBPort;
    stc_gpio_config_t         stcOutPort;
    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIMACfg);
    DDL_ZERO_STRUCT(stcAdtTIMBCfg);
    DDL_ZERO_STRUCT(stcAdtPortTrig);
    DDL_ZERO_STRUCT(stcTIM5APort);
    DDL_ZERO_STRUCT(stcTIM5BPort);
    DDL_ZERO_STRUCT(stcTRIBPort);
    DDL_ZERO_STRUCT(stcOutPort);
    
    
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);         //????????????????????????
        
    //PA04?????????TIM5_CHA
    //Gpio_ClrAnalogMode(GpioPortA, GpioPin4);
    stcTIM5APort.enDir  = GpioDirOut;
    Gpio_Init(GpioPortA, GpioPin4, &stcTIM5APort);
    Gpio_SetAfMode(GpioPortA,GpioPin4,GpioAf5);
    
    //PA05?????????TIM5_CHB
    //Gpio_ClrAnalogMode(GpioPortA, GpioPin5);
    stcTIM5BPort.enDir  = GpioDirOut;
    Gpio_Init(GpioPortA, GpioPin5, &stcTIM5BPort);
    Gpio_SetAfMode(GpioPortA,GpioPin5,GpioAf5);
    
    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE))//ADT??????????????????
    {
        return Error;
    }
    
    enAdt = AdtTIM5;
    
    stcAdtBaseCntCfg.enCntMode = AdtTriangleModeA;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;
    Adt_Init(enAdt, &stcAdtBaseCntCfg);                      //ADT????????????????????????????????????

    /******************************??????????????????********************************/
//Q??????164973950
//TEL???024-85718900
    stcAdtTIMACfg.enCap = AdtCHxCompareOutput;
    stcAdtTIMACfg.bOutEn = FALSE;
    stcAdtTIMBCfg.enCap = AdtCHxCompareOutput;
    stcAdtTIMBCfg.bOutEn = FALSE;
    Adt_CHxXPortConfig(enAdt, AdtCHxA, &stcAdtTIMACfg);        //TIM5_CHA????????????
    Adt_CHxXPortConfig(enAdt, AdtCHxB, &stcAdtTIMBCfg);        //TIM5_CHA????????????

    
    u16Period = 0xA000;
    Adt_SetPeriod(enAdt, u16Period);                           //????????????

    enAdtCompare = AdtCompareA;
    u16Compare = 0x1000;
    Adt_SetCompareValue(enAdt, enAdtCompare, u16Compare);      //??????????????????????????????A??????

    enAdtCompare = AdtCompareB;
    u16Compare = 0x3000;
    Adt_SetCompareValue(enAdt, enAdtCompare, u16Compare);       //??????????????????????????????B??????
    
    enAdtCompare = AdtCompareC;
    u16Compare = 0x5000;
    Adt_SetCompareValue(enAdt, enAdtCompare, u16Compare);       //??????????????????????????????C??????
    
    enAdtCompare = AdtCompareD;
    u16Compare = 0x7000;
    Adt_SetCompareValue(enAdt, enAdtCompare, u16Compare);       //??????????????????????????????D??????
    
    Adt_ConfigIrq(enAdt, AdtCMAIrq, TRUE, Adt5CompACalllback);
    Adt_ConfigIrq(enAdt, AdtCMBIrq, TRUE, Adt5CompBCalllback);
    Adt_ConfigIrq(enAdt, AdtCMCIrq, TRUE, Adt5CompCCalllback);
    Adt_ConfigIrq(enAdt, AdtCMDIrq, TRUE, Adt5CompDCalllback);  //??????????????????
    
    Adt_StartCount(enAdt);
    
    delay1ms(1000);
    
    Adt_ConfigIrq(enAdt, AdtCMAIrq, FALSE, Adt5CompACalllback);
    Adt_ConfigIrq(enAdt, AdtCMBIrq, FALSE, Adt5CompBCalllback);
    Adt_ConfigIrq(enAdt, AdtCMCIrq, FALSE, Adt5CompCCalllback);
    Adt_ConfigIrq(enAdt, AdtCMDIrq, FALSE, Adt5CompDCalllback); //??????????????????
    Adt_StopCount(enAdt);
    Adt_ClearCount(enAdt);
    
    
    /*******************************??????????????????*******************************/
//Q??????164973950
//TEL???024-85718900
    
    // PB14 -> PB11 Rise -> Time TrigB Rise -> HW Capture A
    // PB14 -> PB11 Fall -> Time TrigB Fall -> HW Capture B
        
    //PB11?????????ADT TRIB ??????
    //Gpio_ClrAnalogMode(GpioPortB, GpioPin11);
    stcTRIBPort.enDir  = GpioDirIn;
    Gpio_Init(GpioPortB, GpioPin11, &stcTRIBPort);

    
    //PB14????????????????????????
    //Gpio_ClrAnalogMode(GpioPortB, GpioPin14);
    stcOutPort.enDir  = GpioDirOut;
    Gpio_Init(GpioPortB, GpioPin14, &stcOutPort);

    Gpio_WriteOutputIO(GpioPortB, GpioPin14, FALSE);                //PB14?????????
    
    stcAdtPortTrig.enTrigSrc = AdtTrigxSelPB11;                     //PB11??????TRIB??????
    stcAdtPortTrig.bFltEn = TRUE;                                   //??????????????????
    stcAdtPortTrig.enFltClk = AdtFltClkPclk0Div16;                  //????????????
    Adt_PortTrigConfig(AdtTrigB, &stcAdtPortTrig);                  //??????????????????
    Adt_ConfigHwCaptureA(AdtTIM5, AdtHwTrigTimTriBRise);             //????????????A????????????TRIB?????????????????????
    Adt_ConfigHwCaptureB(AdtTIM5, AdtHwTrigTimTriBFall);             //????????????B????????????TRIB?????????????????????
    Adt_ConfigIrq(AdtTIM5, AdtCMAIrq, TRUE, Adt5CaptureACalllback);
    Adt_ConfigIrq(AdtTIM5, AdtCMBIrq, TRUE, Adt5CaptureBCalllback);  //??????????????????
    
    Adt_StartCount(enAdt);
    delay1ms(1000);
    Gpio_WriteOutputIO(GpioPortB, GpioPin14, TRUE);     //PB14?????????
    delay1ms(1000);
    Gpio_WriteOutputIO(GpioPortB, GpioPin14, FALSE);     //PB14?????????
    delay1ms(1000);
    
    Adt_ConfigIrq(AdtTIM5, AdtCMAIrq, FALSE, Adt5CaptureACalllback);
    Adt_ConfigIrq(AdtTIM5, AdtCMBIrq, FALSE, Adt5CaptureBCalllback);//??????????????????
    Adt_ClearHwCaptureA(enAdt);
    Adt_ClearHwCaptureB(enAdt);
    Adt_StopCount(enAdt);
    Adt_ClearCount(enAdt);


    /***************************?????????????????????**********************************/
//Q??????164973950
//TEL???024-85718900
    Adt_ConfigIrq(enAdt, AdtOVFIrq, TRUE, Adt5OVFCalllback);//??????????????????
    Adt_ConfigIrq(enAdt, AdtUDFIrq, TRUE, Adt5UDFCalllback);//??????????????????
    
    Adt_StartCount(enAdt);
    
    delay1ms(2000);

    Adt_ConfigIrq(enAdt, AdtOVFIrq, FALSE, Adt5OVFCalllback);//??????????????????
    Adt_ConfigIrq(enAdt, AdtUDFIrq, FALSE, Adt5UDFCalllback);//??????????????????
    
    Adt_StopCount(enAdt);
    Adt_ClearCount(enAdt);
    
    
    /****************************??????????????????????????????****************************/
//Q??????164973950
//TEL???024-85718900
        
    enAdtCompare = AdtCompareA;
    u16Compare = 0x5000;
    Adt_SetCompareValue(enAdt, enAdtCompare, u16Compare);//??????????????????????????????A??????
    DDL_ZERO_STRUCT(stcAdtTIMBCfg);
    stcAdtTIMBCfg.enCap = AdtCHxCompareOutput;
    stcAdtTIMBCfg.bOutEn = TRUE;
    stcAdtTIMBCfg.enPerc = AdtCHxPeriodKeep;
    stcAdtTIMBCfg.enCmpc = AdtCHxCompareInv;
    stcAdtTIMBCfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIMBCfg.enStaOut = AdtCHxPortOutHigh;
    Adt_CHxXPortConfig(enAdt, AdtCHxB, &stcAdtTIMBCfg);//CHxB????????????
    
    Adt_SetDTUA(enAdt, 0x6666);
    Adt_SetDTDA(enAdt, 0x6666);
    Adt_ConfigDT(enAdt, TRUE, TRUE);                     //??????????????????
    
    Adt_ConfigIrq(enAdt, AdtDTEIrq, TRUE, Adt5DTECalllback);//??????????????????
    
    Adt_StartCount(enAdt);
    
    delay1ms(1000);
    Adt_ConfigIrq(enAdt, AdtDTEIrq, FALSE, Adt5DTECalllback);//??????????????????
    Adt_ConfigDT(enAdt, FALSE, FALSE);
    
    Adt_StopCount(enAdt);
    Adt_ClearCount(enAdt);
    
    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900


