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
//QȺ��164973950
//TEL��024-85718900
/******************************************************************************/
//QȺ��164973950
//TEL��024-85718900
/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2017-05-17  1.0  CJ First version for Device Driver Library of Module.
 **
 ******************************************************************************/
//QȺ��164973950
//TEL��024-85718900

/******************************************************************************
 * Include files
 ******************************************************************************/
//QȺ��164973950
//TEL��024-85718900

#include "wdt.h"
#include "lpm.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/
//QȺ��164973950
//TEL��024-85718900

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
//QȺ��164973950
//TEL��024-85718900

/******************************************************************************
 * Local type definitions ('typedef')                                         
 ******************************************************************************/
//QȺ��164973950
//TEL��024-85718900

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
//QȺ��164973950
//TEL��024-85718900

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
//QȺ��164973950
//TEL��024-85718900

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                             
 ******************************************************************************/
//QȺ��164973950
//TEL��024-85718900
#define     T1_PORT                 (0)
#define     T1_PIN                  (3)

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
//QȺ��164973950
//TEL��024-85718900
/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/
//QȺ��164973950
//TEL��024-85718900
uint8_t u32CountWdt;
uint8_t Data=0xff;
uint8_t cnt=0;

static void WdtCallback(void)
{
    Data = ~Data;
    Gpio_WriteOutputIO(GpioPortD,GpioPin5,Data);
    u32CountWdt++;
}
int32_t main(void)
{
    stc_wdt_config_t  stcWdt_Config;
    stc_lpm_config_t stcLpmCfg;
    stc_gpio_config_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcWdt_Config);
    DDL_ZERO_STRUCT(stcLpmCfg);
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); 
    Sysctrl_SetPeripheralGate(SysctrlPeripheralWdt,TRUE);//
    
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_WriteOutputIO(GpioPortD,GpioPin5,0);
    Gpio_Init(GpioPortD,GpioPin5,&stcGpioCfg);
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(GpioPortD,GpioPin4,&stcGpioCfg);
    
    stcLpmCfg.enSLEEPDEEP = SlpDpDisable;//SlpDpEnable;
    stcLpmCfg.enSLEEPONEXIT = SlpExtEnable;
    
    stcWdt_Config.u8LoadValue = 0x0b;//3.2s
    stcWdt_Config.enResetEnable = WINT_EN;//WINT_EN;//WRESET_EN;////
    stcWdt_Config.pfnWdtIrqCb = WdtCallback;
    
    
    Wdt_Init(&stcWdt_Config);
    
    Wdt_Start();
#if 0 
    while(1 == Gpio_GetInputIO(GpioPortD,GpioPin4));//  
    Lpm_Config(&stcLpmCfg);
    Lpm_GotoLpmMode();
#endif
    while (1)
    {
        //cnt = Wdt_ReadWdtValue();
        //Wdt_Feed();//
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//QȺ��164973950
//TEL��024-85718900


