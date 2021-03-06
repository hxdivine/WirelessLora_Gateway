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
#include "adt.h"
#include "adc.h"
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
uint16_t u16AdcRestult0;
uint16_t u16AdcRestult2;
   
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

void AdcContIrqCallback(void)
{    
    Adc_GetSqrResult(&u16AdcRestult0, 0);
    Adc_GetSqrResult(&u16AdcRestult2, 1);

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
    
    DDL_ZERO_STRUCT(stcAdcCfg);
    DDL_ZERO_STRUCT(stcAdcIrq);
    DDL_ZERO_STRUCT(stcAdcIrqCalbaks);
    DDL_ZERO_STRUCT(stcAdcExtTrigCfg);
    DDL_ZERO_STRUCT(stcAdcAN0Port);
    DDL_ZERO_STRUCT(stcAdcAN2Port);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    Gpio_SetAnalogMode(GpioPortA, GpioPin0);        //PA00
    stcAdcAN0Port.enDir = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin0, &stcAdcAN0Port);
        
    Gpio_SetAnalogMode(GpioPortA, GpioPin2);        //PA02
    stcAdcAN2Port.enDir = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin2, &stcAdcAN2Port);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE);
    
    //ADC??????
    Adc_Enable();
    M0P_BGR->CR_f.BGR_EN = 0x1u;  //BGR????????????
    M0P_BGR->CR_f.TS_EN  = 0x0u;
    delay100us(10);
    
    stcAdcCfg.enAdcOpMode      = AdcSCanMode;           //??????????????????
    stcAdcCfg.enAdcClkDiv      = AdcClkSysTDiv1;        //Adc???????????? PCLK
    stcAdcCfg.enAdcSampTimeSel = AdcSampTime8Clk;       //???????????? 6?????????
    stcAdcCfg.enAdcRefVolSel   = RefVolSelAVDD;         //??????AVDD
    stcAdcCfg.bAdcInBufEn      = FALSE;
    
    Adc_Init(&stcAdcCfg);                               //Adc?????????
    
    Adc_ConfigSqrChannel(CH0MUX, AdcExInputCH0);        //??????????????????????????????
    Adc_ConfigSqrChannel(CH1MUX, AdcExInputCH2);        //????????????CH1 --> CH0
    
    EnableNvic(ADC_IRQn, IrqLevel1, TRUE);              //Adc?????????
    
    Adc_EnableIrq();                                    //??????Adc??????
    
    stcAdcIrq.bAdcSQRIrq = TRUE;
    stcAdcIrqCalbaks.pfnAdcSQRIrq = AdcContIrqCallback;
    Adc_ConfigIrq(&stcAdcIrq, &stcAdcIrqCalbaks);       //????????????????????????
    
    u8AdcScanCnt = 2;                                   //????????????2???(2-1????????????????????????)
    
    Adc_ConfigSqrMode(&stcAdcCfg, u8AdcScanCnt, FALSE); //??????????????????????????????
    
    stcAdcExtTrigCfg.enAdcExtTrigRegSel = AdcExtTrig0;
    stcAdcExtTrigCfg.enAdcTrig0Sel      = AdcTrigTimer4;
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
    stc_adt_basecnt_cfg_t     stcAdtBaseCntCfg;
    stc_adt_irq_trig_cfg_t    stcAdtIrqTrigCfg;
    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtIrqTrigCfg);
    
    Sysctrl_SetRCLTrim(SysctrlRclFreq32768);
    Sysctrl_ClkSourceEnable(SysctrlClkRCL, TRUE);
    Sysctrl_SysClkSwitch(SysctrlClkRCL);
    
    ///< ?????????????????????RCH???TRIM???
    Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);
    ///< ??????RCH???????????????????????????????????????????????????
    //Sysctrl_ClkSourceEnable(SysctrlClkRCH, TRUE);
    ///< ???????????????RCH
    Sysctrl_SysClkSwitch(SysctrlClkRCH);
    ///< ??????RCL??????
    Sysctrl_ClkSourceEnable(SysctrlClkRCL, FALSE);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);    //????????????????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE);  //ADT??????????????????
    

    
    //ADT Timer4??????ADC?????????Frequency: 50K
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;              //?????????
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;                //PCLK
    
    Adt_Init(AdtTIM4, &stcAdtBaseCntCfg);                      //ADT????????????????????????????????????
    
    u16Period = 480;                                           //50K: 24M * 20us = 480
    Adt_SetPeriod(AdtTIM4, u16Period);                         //????????????

    stcAdtIrqTrigCfg.bAdtOverFlowTrigEn = TRUE;
    Adt_IrqTrigConfig(AdtTIM4, &stcAdtIrqTrigCfg);            //??????????????????ADC??????
    
    ConfigAdc();
    
    Adt_StartCount(AdtTIM4);                                  //timer4??????
    
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


