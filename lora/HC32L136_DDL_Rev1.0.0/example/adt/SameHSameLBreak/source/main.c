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
#include "flash.h"

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


/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
 void AdtIntSMAH(void)
 {
     
 }

 void AdtIntSMAL(void)
 {
     
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
    en_adt_unit_t            enAdt;
    uint16_t                 u16Period;
    uint16_t                 u16CompareA;
    uint16_t                 u16CompareB;
    en_flash_waitcycle_t     enFlashWait;
    stc_sysctrl_clk_config_t stcClkConfig;
    stc_adt_basecnt_cfg_t    stcAdtBaseCntCfg;
    stc_adt_CHxX_port_cfg_t  stcAdtTIM4ACfg;
    stc_adt_CHxX_port_cfg_t  stcAdtTIM4BCfg;
    stc_adt_disable_1_cfg_t  stcAdtDisable1;
    stc_gpio_config_t        stcTIM4APort;
    stc_gpio_config_t        stcTIM4BPort;
    
    DDL_ZERO_STRUCT(stcClkConfig);
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIM4ACfg);
    DDL_ZERO_STRUCT(stcAdtTIM4BCfg);
    DDL_ZERO_STRUCT(stcAdtDisable1);
    DDL_ZERO_STRUCT(stcTIM4APort);
    DDL_ZERO_STRUCT(stcTIM4BPort);
    
    enFlashWait = FlashWaitCycle1;                      //????????????????????????1??????HCLK??????24MHz??????????????????1???
    Flash_WaitCycle(enFlashWait);                       // Flash ??????1?????????
    
    stcClkConfig.enClkSrc    = SysctrlClkXTH;
    stcClkConfig.enHClkDiv   = SysctrlHclkDiv1;
    stcClkConfig.enPClkDiv   = SysctrlPclkDiv1;
    Sysctrl_SetXTHFreq(SysctrlXthFreq20_32MHz);
    Sysctrl_ClkInit(&stcClkConfig);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //????????????????????????

    //PA08?????????TIM4_CHA
    //Gpio_ClrAnalogMode(GpioPortA, GpioPin8);
    stcTIM4APort.enDir  = GpioDirOut;
    Gpio_Init(GpioPortA, GpioPin8, &stcTIM4APort);
    Gpio_SetAfMode(GpioPortA,GpioPin8,GpioAf6);
    
    //PA07?????????TIM4_CHB
    //Gpio_ClrAnalogMode(GpioPortA, GpioPin7);
    stcTIM4BPort.enDir  = GpioDirOut;
    Gpio_Init(GpioPortA, GpioPin7, &stcTIM4BPort);
    Gpio_SetAfMode(GpioPortA,GpioPin7,GpioAf7);
    
    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE))//ADT??????????????????
    {
        return Error;
    }
    
    enAdt = AdtTIM4;  
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div64;
    Adt_Init(enAdt, &stcAdtBaseCntCfg);                      //ADT????????????????????????????????????
    
    /*****************************????????????*************************************/
//Q??????164973950
//TEL???024-85718900
    
    u16Period = 0xC000;
    Adt_SetPeriod(enAdt, u16Period);                         //????????????
    
    u16CompareA = 0x6000;
    Adt_SetCompareValue(enAdt, AdtCompareA, u16CompareA);    //??????????????????????????????A??????
    
    u16CompareB = 0x8000;
    Adt_SetCompareValue(enAdt, AdtCompareB, u16CompareB);    //??????????????????????????????B??????
    
        
    stcAdtTIM4ACfg.enCap = AdtCHxCompareOutput;              //????????????
    stcAdtTIM4ACfg.bOutEn = TRUE;                            //CHA????????????
    stcAdtTIM4ACfg.enPerc = AdtCHxPeriodLow;                 //???????????????????????????CHA???????????????
    stcAdtTIM4ACfg.enCmpc = AdtCHxCompareHigh;               //?????????????????????A????????????CHA???????????????
    stcAdtTIM4ACfg.enStaStp = AdtCHxStateSelSS;              //CHA?????????????????????STACA???STPCA??????
    stcAdtTIM4ACfg.enStaOut = AdtCHxPortOutLow;              //CHA??????????????????
    stcAdtTIM4ACfg.enStpOut = AdtCHxPortOutLow;              //CHA??????????????????
    stcAdtTIM4ACfg.enDisSel = AdtCHxDisSel1;                 //????????????1(??????????????????)
    stcAdtTIM4ACfg.enDisVal = AdtTIMxDisValLow;              //????????????CHA?????????
    Adt_CHxXPortConfig(enAdt, AdtCHxA, &stcAdtTIM4ACfg);     //??????CHA??????
                                
    stcAdtTIM4BCfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4BCfg.bOutEn = TRUE;
    stcAdtTIM4BCfg.enPerc = AdtCHxPeriodLow;
    stcAdtTIM4BCfg.enCmpc = AdtCHxCompareHigh;
    stcAdtTIM4BCfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4BCfg.enStaOut = AdtCHxPortOutLow;
    stcAdtTIM4BCfg.enStpOut = AdtCHxPortOutLow;
    stcAdtTIM4BCfg.enDisSel = AdtCHxDisSel1;
    stcAdtTIM4BCfg.enDisVal = AdtTIMxDisValLow;              //????????????CHA?????????
    Adt_CHxXPortConfig(enAdt, AdtCHxB, &stcAdtTIM4BCfg);     //??????CHB??????
    
    //Adt_ConfigIrq(enAdt, AdtSAMHIrq, TRUE, AdtIntSMAH);      //??????????????????
    
    Adt_StartCount(enAdt);                                   //TIM4??????
    
    delay1ms(2000);
    
    DDL_ZERO_STRUCT(stcAdtDisable1);
    stcAdtDisable1.bTim4OutSH = TRUE;                       //Timer4????????????????????????
    Adt_Disable1Cfg(&stcAdtDisable1);                       //????????????1??????
    
    while(!Adt_GetSameBrakeFlag());                         //????????????????????????
    
    Adt_StopCount(enAdt);                                   //TIM4??????
    
    delay1ms(2000);
    
    //Adt_ConfigIrq(enAdt, AdtSAMHIrq, FALSE, AdtIntSMAH);    //??????????????????
    
    stcAdtDisable1.bTim4OutSH = FALSE;                      //Timer4????????????????????????
    Adt_Disable1Cfg(&stcAdtDisable1);                       //????????????1??????
    Adt_ClearSameBrakeFlag();
    
    Adt_ClearCount(enAdt);

    /*****************************????????????*************************************/
//Q??????164973950
//TEL???024-85718900
    
    u16Period = 0xC000;
    Adt_SetPeriod(enAdt, u16Period);
    
    u16CompareA = 0x6000;
    Adt_SetCompareValue(enAdt, AdtCompareA, u16CompareA);
    
    u16CompareB = 0xA000;
    Adt_SetCompareValue(enAdt, AdtCompareB, u16CompareB);
    
    stcAdtTIM4ACfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4ACfg.bOutEn = TRUE;
    stcAdtTIM4ACfg.enPerc = AdtCHxPeriodHigh;                 //???????????????????????????CHA???????????????
    stcAdtTIM4ACfg.enCmpc = AdtCHxCompareLow;                 //?????????????????????A????????????CHA???????????????
    stcAdtTIM4ACfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4ACfg.enStaOut = AdtCHxPortOutHigh;              //CHA??????????????????
    stcAdtTIM4ACfg.enStpOut = AdtCHxPortOutHigh;              //CHA??????????????????
    stcAdtTIM4ACfg.enDisSel = AdtCHxDisSel1;
    stcAdtTIM4ACfg.enDisVal = AdtTIMxDisValLow;
    Adt_CHxXPortConfig(enAdt, AdtCHxA, &stcAdtTIM4ACfg);
                                
    stcAdtTIM4BCfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4BCfg.bOutEn = TRUE;
    stcAdtTIM4BCfg.enPerc = AdtCHxPeriodHigh;                 //???????????????????????????CHA???????????????
    stcAdtTIM4BCfg.enCmpc = AdtCHxCompareLow;                 //?????????????????????A????????????CHA???????????????
    stcAdtTIM4BCfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4BCfg.enStaOut = AdtCHxPortOutHigh;              //CHA??????????????????
    stcAdtTIM4BCfg.enStpOut = AdtCHxPortOutHigh;              //CHA??????????????????
    stcAdtTIM4BCfg.enDisSel = AdtCHxDisSel1;
    stcAdtTIM4BCfg.enDisVal = AdtTIMxDisValLow;
    Adt_CHxXPortConfig(enAdt, AdtCHxB, &stcAdtTIM4BCfg);
    
    //Adt_ConfigIrq(enAdt, AdtSAMLIrq, TRUE, AdtIntSMAL);      //??????????????????
    
    Adt_StartCount(enAdt);                                   //TIM4??????
    
    delay1ms(2000);
    
    DDL_ZERO_STRUCT(stcAdtDisable1);
    stcAdtDisable1.bTim4OutSL = TRUE;                       //Timer4????????????????????????
    Adt_Disable1Cfg(&stcAdtDisable1);                       //????????????1??????
    
    while(!Adt_GetSameBrakeFlag());
    
    Adt_StopCount(enAdt);
    
    delay1ms(2000);
    
    //Adt_ConfigIrq(enAdt, AdtSAMLIrq, FALSE, AdtIntSMAL);    //??????????????????
    
    stcAdtDisable1.bTim4OutSL = FALSE;                      //Timer4????????????????????????
    Adt_Disable1Cfg(&stcAdtDisable1);                       //????????????1??????
    Adt_ClearSameBrakeFlag();
    
    Adt_ClearCount(enAdt);
    
    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900


