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
 **   - 2017-05-28 LiuHL    First Version
 **
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900

/******************************************************************************
 * Include files
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
#include "adc.h"
#include "gpio.h"
#include "bgr.h"
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
uint16_t u16AdcRestult0;
uint16_t u16AdcRestult2;
uint16_t u16AdcRestult5;
   
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
static stc_adc_irq_t stcAdcIrqFlag;


/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
void AdcRegCmpIrqCallback(void)
{    
    Adc_GetSqrResult(&u16AdcRestult0, 0);
    Adc_GetSqrResult(&u16AdcRestult2, 1);
    Adc_GetSqrResult(&u16AdcRestult5, 2);
  
    stcAdcIrqFlag.bAdcRegCmp = TRUE;
}
void AdcContIrqCallback(void)
{    
    Adc_GetSqrResult(&u16AdcRestult0, 0);
    Adc_GetSqrResult(&u16AdcRestult2, 1);
    Adc_GetSqrResult(&u16AdcRestult5, 2);
  
    stcAdcIrqFlag.bAdcSQRIrq = TRUE;
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
    uint8_t                    u8AdcScanCnt;
    //uint32_t                   u32AdcResultAcc;
    stc_adc_cfg_t              stcAdcCfg;
    stc_adc_irq_t              stcAdcIrq;
    stc_adc_irq_calbakfn_pt_t  stcAdcIrqCalbaks;
    stc_gpio_config_t          stcAdcAN0Port;
    stc_gpio_config_t          stcAdcAN2Port;
    stc_gpio_config_t          stcAdcAN5Port;
    stc_adc_threshold_cfg_t    stcAdcThrCfg;
    
    DDL_ZERO_STRUCT(stcAdcCfg);
    DDL_ZERO_STRUCT(stcAdcIrq);
    DDL_ZERO_STRUCT(stcAdcIrqCalbaks);
    DDL_ZERO_STRUCT(stcAdcIrqFlag);
    DDL_ZERO_STRUCT(stcAdcAN0Port);
    DDL_ZERO_STRUCT(stcAdcAN2Port);
    DDL_ZERO_STRUCT(stcAdcAN5Port);
    
        
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    Gpio_SetAnalogMode(GpioPortA, GpioPin0);        //PA00 (AIN0)
    stcAdcAN0Port.enDir = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin0, &stcAdcAN0Port);

    
    Gpio_SetAnalogMode(GpioPortA, GpioPin2);        //PA02 (AIN2)
    stcAdcAN2Port.enDir = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin2, &stcAdcAN2Port);

    
    
    Gpio_SetAnalogMode(GpioPortA, GpioPin5);        //PA05 (AIN5)
    stcAdcAN5Port.enDir = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin5, &stcAdcAN5Port);
 
    
    
    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE))
    {
        return 1;
    }    
    
    //ADC配置
    Adc_Enable();
    Bgr_BgrEnable();
    delay100us(1);
    
    stcAdcCfg.enAdcOpMode = AdcSCanMode;         //扫描模式
    stcAdcCfg.enAdcClkDiv = AdcClkSysTDiv1;
    stcAdcCfg.enAdcSampTimeSel = AdcSampTime8Clk;
    stcAdcCfg.enAdcRefVolSel   = RefVolSelAVDD;
    stcAdcCfg.bAdcInBufEn      = FALSE;
    
    Adc_Init(&stcAdcCfg);
    
    Adc_ConfigSqrChannel(CH0MUX, AdcExInputCH0);
    Adc_ConfigSqrChannel(CH1MUX, AdcExInputCH2);
    Adc_ConfigSqrChannel(CH2MUX, AdcExInputCH5);
    
    stcAdcThrCfg.bAdcRegCmp = TRUE;
    stcAdcThrCfg.bAdcHhtCmp = FALSE;
    stcAdcThrCfg.bAdcLltCmp = FALSE;
    stcAdcThrCfg.u32AdcRegHighThd = 0xC00; 
    stcAdcThrCfg.u32AdcRegLowThd  = 0x200;
    stcAdcThrCfg.enThCh =  AdcExInputCH0;
    Adc_ThresholdCfg(&stcAdcThrCfg);
    
    EnableNvic(ADC_IRQn, IrqLevel3, TRUE);
    
    Adc_EnableIrq();
    
    stcAdcIrq.bAdcSQRIrq = TRUE;
    stcAdcIrqCalbaks.pfnAdcSQRIrq = AdcContIrqCallback;
    
    Adc_ConfigIrq(&stcAdcIrq, &stcAdcIrqCalbaks);
    
    stcAdcIrq.bAdcRegCmp = TRUE;
    stcAdcIrqCalbaks.pfnAdcRegIrq = AdcRegCmpIrqCallback;
    
    Adc_ConfigIrq(&stcAdcIrq, &stcAdcIrqCalbaks);
    
    u8AdcScanCnt = 3;
    
    Adc_ConfigSqrMode(&stcAdcCfg, u8AdcScanCnt, FALSE);
    
    Adc_SQR_Start();
    
    while(1)
    {        
        while(FALSE == stcAdcIrqFlag.bAdcSQRIrq);

        stcAdcIrqFlag.bAdcSQRIrq = FALSE;
        
        Adc_SQR_Start();
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900


