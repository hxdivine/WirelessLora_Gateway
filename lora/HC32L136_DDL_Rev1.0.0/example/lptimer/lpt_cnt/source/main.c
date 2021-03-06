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
#include "lpt.h"
#include "lpm.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900

#define LED_PORT     GpioPortD
#define LED_PIN      GpioPin5

#define Button_PORT  GpioPortD
#define Button_PIN   GpioPin4

#define LptETR_PORT  GpioPortB
#define LptETR_PIN   GpioPin4

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

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
static volatile uint32_t u32LptTestFlag = 0;


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
 * Lpt??????????????????
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
void LptInt(void)
{
    static uint8_t i;
    
    if (TRUE == Lpt_GetIntFlag())
    {
        Lpt_ClearIntFlag();
        u32LptTestFlag = 0x01;
        
        if(i%2 == 0)
        {
            Gpio_WriteOutputIO(LED_PORT, LED_PIN, TRUE);     //??????????????????LED
        }
        else
        {
            Gpio_WriteOutputIO(LED_PORT, LED_PIN, FALSE);    //??????????????????LED
        }
        i++;
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
    stc_lpt_config_t          stcConfig;
    stc_lpm_config_t          stcLpmCfg;
    stc_gpio_config_t         stcLEDPort;
    stc_gpio_config_t         stcButtonPort;
    stc_gpio_config_t         stcETRPort;
    stc_sysctrl_clk_config_t  stcClkCfg;
    uint16_t                  u16ArrData = 0xE000;
    volatile uint8_t          u8TestFlag = 0;
    
    DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcLEDPort);
    DDL_ZERO_STRUCT(stcButtonPort);
    DDL_ZERO_STRUCT(stcETRPort);
    DDL_ZERO_STRUCT(stcLpmCfg);
    DDL_ZERO_STRUCT(stcClkCfg);
    
    //CLK INIT
    stcClkCfg.enClkSrc  = SysctrlClkRCH;
    stcClkCfg.enHClkDiv = SysctrlHclkDiv1;
    stcClkCfg.enPClkDiv = SysctrlPclkDiv1;
    Sysctrl_ClkInit(&stcClkCfg);
    
    //??????RCL
    Sysctrl_ClkSourceEnable(SysctrlClkRCL, TRUE);
    
#if 0
    M0P_SYSCTRL->RCL_CR_f.STARTUP = 3;
    M0P_SYSCTRL->RCL_CR_f.TRIM      = RCL_CR_TRIM_32_8K_VAL;
    M0P_SYSCTRL->SYSCTRL2_f.SYSCTRL2 = 0x5A5A;
    M0P_SYSCTRL->SYSCTRL2_f.SYSCTRL2 = 0xA5A5;
    M0P_SYSCTRL->SYSCTRL0_f.RCL_EN   = 1;
    while(1 != M0P_SYSCTRL->RCL_CR_f.STABLE);
#endif 
    
    //??????Lpt,GPIO????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralLpTim, TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
        
    //PB04??????LPT??????????????????????????????
    //Gpio_ClrAnalogMode(LptETR_PORT, LptETR_PIN);
    stcETRPort.enDrv  = GpioDrvH;
    stcETRPort.enDir  = GpioDirIn;
    Gpio_Init(LptETR_PORT, LptETR_PIN, &stcETRPort);
    Gpio_SetAfMode(LptETR_PORT,LptETR_PIN,GpioAf7);
    
    //PD05?????????GPIO-->LED?????????
    //Gpio_ClrAnalogMode(LED_PORT, LED_PIN);
    stcLEDPort.enDrv  = GpioDrvH;
    stcLEDPort.enDir  = GpioDirOut;
    Gpio_Init(LED_PORT, LED_PIN, &stcLEDPort);

    //PD04?????????GPIO<--SW2?????????
    //Gpio_ClrAnalogMode(Button_PORT, Button_PIN);
    stcButtonPort.enDrv  = GpioDrvH;
    stcButtonPort.enDir  = GpioDirIn;
    Gpio_Init(Button_PORT, Button_PIN, &stcButtonPort);
    
    Gpio_WriteOutputIO(LED_PORT, LED_PIN, TRUE);     //??????????????????LED
    
    stcConfig.pfnLpTimCb = LptInt;           //??????????????????
    
    stcConfig.enGate   = LptGateDisable;     //Gate Disable
    stcConfig.enGateP  = LptActLowLevel;     //Gate_P ???????????????
    stcConfig.enTckSel = LptIRC32K;          //LPT????????????RCL
    stcConfig.enTog    = LptTogDisable;      //TOG??????Disable
    stcConfig.enCT     = LptCounter;         //???????????????????????????????????????
    stcConfig.enMD     = Lpt16bitArrMode;    //16???????????????
    
    Lpt_Init(&stcConfig);                    //LPT?????????
 
    Lpt_EnableIrq();
    Lpt_ClearIntFlag();
    EnableNvic(LPTIM_IRQn, IrqLevel3, TRUE);
    
    //??????????????????????????????
    Lpt_ARRSet(u16ArrData);
    
    //Lpm Cfg
    stcLpmCfg.enSEVONPEND   = SevPndDisable;
    stcLpmCfg.enSLEEPDEEP   = SlpDpEnable;  //??????:??????WFI??????????????????;
    stcLpmCfg.enSLEEPONEXIT = SlpExtEnable; //??????:??????????????????????????????,?????????????????????????????????;
    Lpm_Config(&stcLpmCfg);
    
    Lpt_Run();    //LPT ??????
    
    //??????user??????(PD04)?????????????????????????????????????????????
    //????????????????????????????????????????????????????????????SWD??????????????????????????????
    //???????????????????????????????????????????????????????????????user??????
    while (1 == Gpio_GetInputIO(Button_PORT,Button_PIN));
    
    M0P_GPIO->PAADS = 0;
    M0P_GPIO->PBADS = 0;
    M0P_GPIO->PDADS = 0;
    if(LptXIL32K == stcConfig.enTckSel)
    {
        M0P_GPIO->PCADS = 0xC000;
    }
    else
    {
        M0P_GPIO->PCADS = 0;
    }
    
    M0P_GPIO->PADIR = 0;
    M0P_GPIO->PBDIR = 0x0010;
    M0P_GPIO->PCDIR = 0;
    M0P_GPIO->PDDIR = 0;
    
    M0P_GPIO->PAOUT = 0;
    M0P_GPIO->PBOUT = 0;
    M0P_GPIO->PCOUT = 0;
    M0P_GPIO->PDOUT = 0;
    //Gpio_ClrPort(GpioPortA,0);
    //Gpio_ClrPort(GpioPortB,0);
    //Gpio_ClrPort(GpioPortC,0);
    //Gpio_ClrPort(GpioPortD,0);
    
    M0P_GPIO->PAPU = 0;
    M0P_GPIO->PBPU = 0;
    M0P_GPIO->PCPU = 0;
    M0P_GPIO->PDPU = 0;
    
    //M0P_GPIO->PDOUT_f.PD05 = 0;
    Gpio_WriteOutputIO(LED_PORT, LED_PIN, FALSE);
    
    Lpm_GotoLpmMode();
        
    while (1)
    {
        if (0x01 == u32LptTestFlag)
        {
            u32LptTestFlag = 0;

        }
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
