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
 **   - 2017-06-26 LiuHL    First Version
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
#include "adc.h"
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
uint16_t u16AdcResult00;
uint16_t u16AdcResult02;
uint16_t u16AdcResult05;

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

void Adt4UnderFullCalllback(void)
{
    //????????????????????????PWM???????????? GCMCR?????????
    //????????????????????????PWM????????? PERBR?????????
}

void AdcJQRIrqCallback(void)
{
    static uint8_t i;
    
    Adc_GetJqrResult(&u16AdcResult00, 0);
    Adc_GetJqrResult(&u16AdcResult02, 1);
    Adc_GetJqrResult(&u16AdcResult05, 2);
    
    //????????????????????????PWM???????????? GCMCR??????????????????

    if(i%2 == 0)
    {
        Adt_SetCompareValue(AdtTIM4, AdtCompareC, 0x5555);
    }
    else
    {
        Adt_SetCompareValue(AdtTIM4, AdtCompareC, 0x9999);
    }
    
    i++;
}


void ConfigAdc(void)
{
    uint8_t                    u8AdcScanCnt;
    stc_adc_cfg_t              stcAdcCfg;
    stc_adc_irq_t              stcAdcIrq;
    stc_adc_irq_calbakfn_pt_t  stcAdcIrqCalbaks;
    stc_adc_ext_trig_cfg_t     stcAdcExtTrigCfg;
    stc_gpio_config_t          stcAdcAN0Port;
    stc_gpio_config_t          stcAdcAN2Port;
    stc_gpio_config_t          stcAdcAN5Port;
    
    
    DDL_ZERO_STRUCT(stcAdcCfg);
    DDL_ZERO_STRUCT(stcAdcIrq);
    DDL_ZERO_STRUCT(stcAdcIrqCalbaks);
    DDL_ZERO_STRUCT(stcAdcExtTrigCfg);
    DDL_ZERO_STRUCT(stcAdcAN0Port);
    DDL_ZERO_STRUCT(stcAdcAN2Port);
    DDL_ZERO_STRUCT(stcAdcAN5Port);
    
        
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    Gpio_SetAnalogMode(GpioPortA, GpioPin0);        //PA00
    stcAdcAN0Port.enDir = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin0, &stcAdcAN0Port);

    
    Gpio_SetAnalogMode(GpioPortA, GpioPin2);        //PA02
    stcAdcAN2Port.enDir = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin2, &stcAdcAN2Port);

    
    
    Gpio_SetAnalogMode(GpioPortA, GpioPin5);        //PA05
    stcAdcAN5Port.enDir = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin5, &stcAdcAN5Port);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE);
    
    //ADC??????
    Adc_Enable();
    M0P_BGR->CR_f.BGR_EN = 0x1u;//BGR????????????
    M0P_BGR->CR_f.TS_EN  = 0x0u;
    delay100us(10);
    
    stcAdcCfg.enAdcOpMode      = AdcSCanMode;           //??????????????????
    stcAdcCfg.enAdcClkDiv      = AdcClkSysTDiv2;        //Adc???????????? PCLK/2
    stcAdcCfg.enAdcSampTimeSel = AdcSampTime6Clk;       //???????????? 6?????????
    stcAdcCfg.enAdcRefVolSel   = RefVolSelAVDD;         //??????AVDD
    stcAdcCfg.bAdcInBufEn      = FALSE;
    
    Adc_Init(&stcAdcCfg);                               //Adc?????????
    
    Adc_ConfigJqrChannel(JQRCH0MUX, AdcExInputCH0);     //??????????????????????????????
    Adc_ConfigJqrChannel(JQRCH1MUX, AdcExInputCH2);
    Adc_ConfigJqrChannel(JQRCH2MUX, AdcExInputCH5);     //ADC????????????CH2 --> CH1 --> CH0
    
    EnableNvic(ADC_IRQn, IrqLevel1, TRUE);              //Adc?????????
    Adc_EnableIrq();                                    //??????Adc??????
    
    stcAdcIrq.bAdcJQRIrq = TRUE;
    stcAdcIrqCalbaks.pfnAdcJQRIrq = AdcJQRIrqCallback;
    Adc_ConfigIrq(&stcAdcIrq, &stcAdcIrqCalbaks);       //????????????????????????
    
    u8AdcScanCnt = 3;                                   //????????????3???(3-1????????????????????????)
    
    Adc_ConfigJqrMode(&stcAdcCfg, u8AdcScanCnt, FALSE); //??????????????????????????????
    
    stcAdcExtTrigCfg.enAdcExtTrigRegSel = AdcExtTrig1;
    stcAdcExtTrigCfg.enAdcTrig1Sel      = AdcTrigTimer4;
    Adc_ExtTrigCfg(&stcAdcExtTrigCfg);                  //Timer4????????????????????????
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
    uint16_t                  u16Period;
    uint16_t                  u16Compare;
    en_flash_waitcycle_t      enFlashWait;
    stc_sysctrl_clk_config_t  stcClkConfig;
    stc_adt_basecnt_cfg_t     stcAdtBaseCntCfg;
    stc_adt_CHxX_port_cfg_t   stcAdtTIM4ACfg;
    stc_adt_CHxX_port_cfg_t   stcAdtTIM4BCfg;
    stc_adt_validper_cfg_t    stcAdtValidPerCfg;
    stc_adt_irq_trig_cfg_t    stcAdtIrqTrigCfg;
    stc_adt_sw_sync_t         stcAdtSwSync;
    stc_gpio_config_t         stcTIM4Port;
    stc_gpio_config_t         stcTIM5Port;
    stc_gpio_config_t         stcTIM6Port;
    
    
    DDL_ZERO_STRUCT(stcClkConfig);
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIM4ACfg);
    DDL_ZERO_STRUCT(stcAdtTIM4BCfg);
    DDL_ZERO_STRUCT(stcAdtSwSync);
    DDL_ZERO_STRUCT(stcTIM4Port);
    DDL_ZERO_STRUCT(stcTIM5Port);
    DDL_ZERO_STRUCT(stcTIM6Port);
    DDL_ZERO_STRUCT(stcAdtValidPerCfg);
    DDL_ZERO_STRUCT(stcAdtIrqTrigCfg);
    
    enFlashWait = FlashWaitCycle1;                      //????????????????????????1??????HCLK??????24MHz??????????????????1???
    Flash_WaitCycle(enFlashWait);                       // Flash ??????1?????????
    
    stcClkConfig.enClkSrc    = SysctrlClkXTH;           //????????????????????????,32M
    stcClkConfig.enHClkDiv   = SysctrlHclkDiv1;         // HCLK = SystemClk/1
    stcClkConfig.enPClkDiv   = SysctrlPclkDiv1;         // PCLK = HCLK/1
    Sysctrl_SetXTHFreq(SysctrlXthFreq20_32MHz);         //???????????????????????????20~32M
    Sysctrl_ClkInit(&stcClkConfig);
    
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //????????????????????????
       
    
    stcTIM4Port.enDir  = GpioDirOut;
    stcTIM5Port.enDir  = GpioDirOut;
    stcTIM6Port.enDir  = GpioDirOut;
    
    Gpio_Init(GpioPortA, GpioPin8, &stcTIM4Port);  
    Gpio_SetAfMode(GpioPortA,GpioPin8,GpioAf6);     //PA08?????????TIM4_CHA
    
    Gpio_Init(GpioPortA, GpioPin7, &stcTIM4Port);
    Gpio_SetAfMode(GpioPortA,GpioPin7,GpioAf7);     //PA07?????????TIM4_CHB
#if 0    
    Gpio_Init(GpioPortA, GpioPin4, &stcTIM5Port);
    Gpio_SetAfMode(GpioPortA,GpioPin4,GpioAf5);     //PA04?????????TIM5_CHA
    
    Gpio_Init(GpioPortA, GpioPin5, &stcTIM5Port);
    Gpio_SetAfMode(GpioPortA,GpioPin5,GpioAf5);     //PA05?????????TIM5_CHB

    Gpio_Init(GpioPortB, GpioPin11, &stcTIM6Port);
    Gpio_SetAfMode(GpioPortB,GpioPin11,GpioAf5);    //PB11?????????TIM6_CHA
    
    Gpio_Init(GpioPortB, GpioPin1, &stcTIM6Port);
    Gpio_SetAfMode(GpioPortB,GpioPin1,GpioAf4);     //PB01?????????TIM6_CHB

#endif
        
    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE))//ADT??????????????????
    {
        return Error;
    }
    
    
    stcAdtBaseCntCfg.enCntMode = AdtTriangleModeA;           //?????????A
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;
    
    Adt_Init(AdtTIM4, &stcAdtBaseCntCfg);                      //ADT????????????????????????????????????
    
    u16Period = 0xEEEE;
    Adt_SetPeriod(AdtTIM4, u16Period);                         //????????????
    Adt_SetPeriodBuf(AdtTIM4, u16Period);                      //?????????????????????????????????????????????????????????
    
    u16Compare = 0x9999;
    Adt_SetCompareValue(AdtTIM4, AdtCompareA, u16Compare);    //??????????????????????????????A??????
    
    
    u16Compare = 0;
    Adt_SetCompareValue(AdtTIM4, AdtCompareB, u16Compare);    //??????????????????????????????B??????

    
    u16Compare = 0x9999;
    Adt_SetCompareValue(AdtTIM4, AdtCompareC, u16Compare);    //??????????????????????????????C??????

    Adt_EnableValueBuf(AdtTIM4, AdtCHxA, TRUE);
    //Adt_EnableValueBuf(AdtTIM4, AdtCHxB, TRUE);

    stcAdtTIM4ACfg.enCap = AdtCHxCompareOutput;            //????????????
    stcAdtTIM4ACfg.bOutEn = TRUE;                          //CHA????????????
    stcAdtTIM4ACfg.enPerc = AdtCHxPeriodKeep;              //???????????????????????????CHA??????????????????
    stcAdtTIM4ACfg.enCmpc = AdtCHxCompareInv;              //?????????????????????A????????????CHA????????????
    stcAdtTIM4ACfg.enStaStp = AdtCHxStateSelSS;            //CHA?????????????????????STACA???STPCA??????
    stcAdtTIM4ACfg.enStaOut = AdtCHxPortOutLow;            //CHA??????????????????
    stcAdtTIM4ACfg.enStpOut = AdtCHxPortOutLow;            //CHA??????????????????
    Adt_CHxXPortConfig(AdtTIM4, AdtCHxA, &stcAdtTIM4ACfg); //??????CHA??????
    
    stcAdtTIM4BCfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4BCfg.bOutEn = TRUE;
    stcAdtTIM4BCfg.enPerc = AdtCHxPeriodKeep;
    stcAdtTIM4BCfg.enCmpc = AdtCHxCompareInv;
    stcAdtTIM4BCfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4BCfg.enStaOut = AdtCHxPortOutHigh;            //CHA??????????????????
    stcAdtTIM4BCfg.enStpOut = AdtCHxPortOutLow;
    Adt_CHxXPortConfig(AdtTIM4, AdtCHxB, &stcAdtTIM4BCfg);   //??????CHB??????

    Adt_SetDTUA(AdtTIM4, 0x2000);
    Adt_SetDTDA(AdtTIM4, 0x2000);
    Adt_ConfigDT(AdtTIM4, TRUE, TRUE);                       //????????????

    Adt_ConfigIrq(AdtTIM4, AdtUDFIrq, TRUE, Adt4UnderFullCalllback);  //??????????????????
    
    stcAdtIrqTrigCfg.bAdtUnderFlowTrigEn = TRUE;
    Adt_IrqTrigConfig(AdtTIM4, &stcAdtIrqTrigCfg);           //??????????????????ADC??????
    
    stcAdtValidPerCfg.enValidCdt = AdtPeriodCnteMax;         //?????????????????????????????????
    stcAdtValidPerCfg.enValidCnt = AdtPeriodCnts1;           //??????1???????????????
    Adt_SetValidPeriod(AdtTIM4, &stcAdtValidPerCfg);         //?????????????????????????????????
    
    ConfigAdc();
    
    stcAdtSwSync.bAdTim4 = TRUE;
    
    Adt_SwSyncStart(&stcAdtSwSync);
    
    while(1)
    {
        //????????????????????????PWM???????????? GCMCR?????????
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900


