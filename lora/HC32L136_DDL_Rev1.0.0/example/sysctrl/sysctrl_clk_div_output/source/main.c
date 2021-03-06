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
#include "sysctrl.h"
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
static void _UserKeyWait(void)
{
    stc_gpio_config_t pstcGpioCfg;
    
    ///< ??????????????????->??????
    pstcGpioCfg.enDir = GpioDirIn;
    ///< ????????????????????????->???????????????
    pstcGpioCfg.enDrv = GpioDrvL;
    ///< ?????????????????????->??????
    pstcGpioCfg.enPuPd = GpioPu;
    ///< ????????????????????????->??????????????????
    pstcGpioCfg.enOD = GpioOdDisable;
    ///< GPIO IO PD04?????????(PD04???STK?????????KEY(USER))
    Gpio_Init(GpioPortD, GpioPin4, &pstcGpioCfg);
    
    while(1)
    {
        ///< ??????PD04??????(USER??????????????????(?????????))
        if(FALSE == Gpio_GetInputIO(GpioPortD, GpioPin4))
        {
            break;
        }
        else
        {
            continue;
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
 ** check Pxx to verify the clock frequency.
 **
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
int32_t main(void)
{
    stc_sysctrl_clk_config_t stcCfg;
    stc_gpio_config_t pstcGpioCfg;
    
    ///< ??????GPIO????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    _UserKeyWait(); //USER KEY???????????????????????????
    
    ///< ????????????????????????HCLK??????24M???????????????FLASH ??????????????????0 cycle(???????????????0 cycle)
    Flash_WaitCycle(FlashWaitCycle0);
    
    ///< ?????????????????????????????????????????????????????????????????????RCH???4MHz???????????????4MHz???
    Sysctrl_SetRCHTrim(SysctrlRchFreq4MHz);
    
    ///< ????????????RCH??????HCLK?????????;
    stcCfg.enClkSrc    = SysctrlClkRCH;
    ///< HCLK SYSCLK/2
    stcCfg.enHClkDiv   = SysctrlHclkDiv2;
    ///< PCLK ???HCLK/8
    stcCfg.enPClkDiv   = SysctrlPclkDiv8;
    ///< ?????????????????????
    Sysctrl_ClkInit(&stcCfg);
    

    ///< ??????????????????->??????
    pstcGpioCfg.enDir = GpioDirOut;
    ///< ????????????????????????->???????????????
    pstcGpioCfg.enDrv = GpioDrvH;
    ///< ?????????????????????->????????????
    pstcGpioCfg.enPuPd = GpioNoPuPd;
    ///< ????????????????????????->??????????????????
    pstcGpioCfg.enOD = GpioOdDisable;
    ///< GPIO IO PB00?????????
    Gpio_Init(GpioPortB, GpioPin0, &pstcGpioCfg);
    ///< GPIO IO PA01?????????
    Gpio_Init(GpioPortA, GpioPin1, &pstcGpioCfg);
    ///< GPIO IO PB01?????????
    Gpio_Init(GpioPortB, GpioPin1, &pstcGpioCfg);
    
    ///< ??????RCH???PB00??????
    Gpio_SetAfMode(GpioPortB, GpioPin0, GpioAf5);
    
    ///< ??????HCLK???PA01??????
    Gpio_SfHClkOutputConfig(GpioSfHclkOutEnable, GpioSfHclkOutDiv1);
    Gpio_SetAfMode(GpioPortA, GpioPin1, GpioAf6);
    
    ///< ??????PCLK???PB01??????
    Gpio_SfPClkOutputConfig(GpioSfPclkOutEnable, GpioSfPclkOutDiv1);
    Gpio_SetAfMode(GpioPortB, GpioPin1, GpioAf2);
    while (1);

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900



