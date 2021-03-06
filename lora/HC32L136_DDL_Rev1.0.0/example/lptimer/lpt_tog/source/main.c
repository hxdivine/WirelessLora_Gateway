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
#define TOG_PORT     GpioPortB
#define TOG_PIN      GpioPin6

#define TOGN_PORT    GpioPortB
#define TOGN_PIN     GpioPin7

#define Button_PORT  GpioPortD
#define Button_PIN   GpioPin4

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
 
    if (TRUE == Lpt_GetIntFlag())
    {
        Lpt_ClearIntFlag();
        u32LptTestFlag = 0x01;
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
    stc_gpio_config_t         stcTOGPort;
    stc_gpio_config_t         stcTOGNPort;
    stc_gpio_config_t         stcButtonPort;
    stc_sysctrl_clk_config_t  stcClkCfg;
    uint16_t                  u16ArrData = 0x8000;
    volatile uint8_t          u8TestFlag = 0;
            
    DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcLpmCfg);
    DDL_ZERO_STRUCT(stcClkCfg);
    DDL_ZERO_STRUCT(stcTOGPort);
    DDL_ZERO_STRUCT(stcTOGNPort);
    DDL_ZERO_STRUCT(stcButtonPort);
    
    //CLK INIT
    stcClkCfg.enClkSrc  = SysctrlClkRCH;
    stcClkCfg.enHClkDiv = SysctrlHclkDiv1;
    stcClkCfg.enPClkDiv = SysctrlPclkDiv1;
    Sysctrl_ClkInit(&stcClkCfg);
    
    //??????RCL
    Sysctrl_ClkSourceEnable(SysctrlClkRCL, TRUE);
    
    //??????Lpt,GPIO????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralLpTim, TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    
    //PB06?????????LP Timer TOG
    //Gpio_ClrAnalogMode(LptETR_PORT, LptETR_PIN);
    stcTOGPort.enDrv  = GpioDrvH;
    stcTOGPort.enDir  = GpioDirOut;
    Gpio_Init(TOG_PORT, TOG_PIN, &stcTOGPort);
    Gpio_SetAfMode(TOG_PORT,TOG_PIN,GpioAf7);
    
    //PB07?????????LP Timer TOGN
    //Gpio_ClrAnalogMode(LED_PORT, LED_PIN);
    stcTOGNPort.enDrv  = GpioDrvH;
    stcTOGNPort.enDir  = GpioDirOut;
    Gpio_Init(TOGN_PORT, TOGN_PIN, &stcTOGNPort);
    Gpio_SetAfMode(TOGN_PORT,TOGN_PIN,GpioAf6);

    //PD04?????????GPIO<--SW2?????????
    //Gpio_ClrAnalogMode(Button_PORT, Button_PIN);
    stcButtonPort.enDrv  = GpioDrvH;
    stcButtonPort.enDir  = GpioDirIn;
    Gpio_Init(Button_PORT, Button_PIN, &stcButtonPort);
    
    stcConfig.pfnLpTimCb = LptInt;           //??????????????????
    
    stcConfig.enGate   = LptGateDisable;     //Gate Disable
    stcConfig.enGateP  = LptActLowLevel;     //Gate_P ???????????????
    stcConfig.enTckSel = LptIRC32K;          //LPT????????????RCL
    stcConfig.enTog    = LptTogEnable;       //TOG??????Enable
    stcConfig.enCT     = LptTimer;           //???????????????
    stcConfig.enMD     = Lpt16bitArrMode;    //???????????????16????????????
    
    Lpt_Init(&stcConfig);                    //LPT?????????
    
    Lpt_ARRSet(u16ArrData);                  //??????????????????
    
    //Lpm Cfg
    stcLpmCfg.enSEVONPEND   = SevPndDisable;
    stcLpmCfg.enSLEEPDEEP   = SlpDpEnable;  //??????:??????WFI??????????????????;
    stcLpmCfg.enSLEEPONEXIT = SlpExtDisable;
    Lpm_Config(&stcLpmCfg);
    
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
    M0P_GPIO->PBDIR = 0;
    M0P_GPIO->PCDIR = 0;
    M0P_GPIO->PDDIR = 0;
    
    M0P_GPIO->PAOUT = 0;
    M0P_GPIO->PBOUT = 0;
    M0P_GPIO->PCOUT = 0;
    M0P_GPIO->PDOUT = 0;
    
    M0P_GPIO->PAPU = 0;
    M0P_GPIO->PBPU = 0;
    M0P_GPIO->PCPU = 0;
    M0P_GPIO->PDPU = 0;
    
    Lpt_Run();    //LPT ??????
    
    
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


