/******************************************************************************
* Copyright (C) 2018, Huada Semiconductor Co.,Ltd All rights reserved.
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
 **   - 2018-05-08  1.0  Lux First version for Device Driver Library of Module.
 **
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900

/******************************************************************************
 * Include files
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
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
void Gpio_IRQHandler(uint8_t u8Param)
{
    ///< PORT D
    if(3 == u8Param)
    {
        if(TRUE == Gpio_GetIrqStatus(GpioPortD, GpioPin4))
        {
            Gpio_ClearIrq(GpioPortD, GpioPin4);
            
            ///< ??????PD05?????????????????????LED?????????
            Gpio_SetIO(GpioPortD, GpioPin5);
            
            delay1ms(2000);
            
            ///< ??????PD05?????????????????????LED?????????
            Gpio_ClrIO(GpioPortD, GpioPin5);        
        }

    }
}    

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900

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
    stc_gpio_config_t pstcGpioCfg;
    
    ///< ??????GPIO??????????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    ///< ??????????????????->??????
    pstcGpioCfg.enDir = GpioDirIn;
    ///< ????????????????????????->???????????????
    pstcGpioCfg.enDrv = GpioDrvL;
    ///< ?????????????????????->??????
    pstcGpioCfg.enPuPd = GpioPu;
    ///< ????????????????????????->??????????????????
    pstcGpioCfg.enOD = GpioOdDisable;
    ///< ????????????/??????????????????????????????????????????->AHB
    pstcGpioCfg.enCtrlMode = GpioAHB;
    
    ///< GPIO IO PD04?????????(PD04???STK?????????KEY(USER))
    Gpio_Init(GpioPortD, GpioPin4, &pstcGpioCfg);
    
    Gpio_ClearIrq(GpioPortD, GpioPin4);
    ///< ???????????????PD04??????????????????
    Gpio_EnableIrq(GpioPortD, GpioPin4, GpioIrqFalling);
    ///< ????????????PORTD????????????
    EnableNvic(PORTD_IRQn, IrqLevel3, TRUE);
 
    ///< ??????????????????->??????(???????????????????????????????????????????????????)
    pstcGpioCfg.enDir = GpioDirOut;
    ///< ?????????????????????->??????
    pstcGpioCfg.enPuPd = GpioPd;
    ///< GPIO IO PD05?????????(PD05???STK?????????LED(LED3))
    Gpio_Init(GpioPortD, GpioPin5, &pstcGpioCfg);
    ///< ??????PD05?????????????????????LED?????????
    Gpio_ClrIO(GpioPortD, GpioPin5);
    
    while(1)
    {
        ;
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900


