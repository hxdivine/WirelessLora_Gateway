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
#include "bt.h"
#include "lpm.h"
#include "gpio.h"
#include "flash.h"
#include "adc.h"

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
uint32_t  Bt_CapTemp_Value1,Bt_CapTemp_Value2;
int32_t   Bt_Capture_Value;
uint16_t  Bt_Uev_Cnt;

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900


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
 * BT2??????????????????
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
void Tim0Int(void)
{
    static uint8_t i;
    
    if(TRUE == Bt_GetIntFlag(TIM0, BtCB0Irq))
    {
        if(0 == i)
        {
            Bt_CapTemp_Value1 = M0P_TIM0_MODE23->CCR0B_f.CCR0B;
            Bt_Uev_Cnt = 0;
            
            i = 1;
        }
        else if(1 == i)
        {
            Bt_CapTemp_Value2 = M0P_TIM0_MODE23->CCR0B_f.CCR0B;
            Bt_Capture_Value = Bt_Uev_Cnt * 0xFFFF + Bt_CapTemp_Value2 - Bt_CapTemp_Value1;
            
            i = 0;
        }
        
        Bt_ClearIntFlag(TIM0,BtCB0Irq);
    }
    if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
    {
        Bt_Uev_Cnt++;
        Bt_ClearIntFlag(TIM0,BtUevIrq);
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
    uint16_t                      u16ArrValue;
    uint16_t                      u16CntValue;
    en_flash_waitcycle_t          enFlashWait;
    stc_sysctrl_clk_config_t      stcClkConfig;
    stc_bt_mode23_config_t        stcBtBaseCfg;
    stc_bt_m23_input_config_t     stcBtPortCapCfg;
    stc_gpio_config_t             stcTIM0Port;
    
    DDL_ZERO_STRUCT(stcClkConfig);
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    DDL_ZERO_STRUCT(stcTIM0Port);
    DDL_ZERO_STRUCT(stcBtPortCapCfg);
    
    enFlashWait = FlashWaitCycle1;                      //????????????????????????1??????HCLK??????24MHz??????????????????1???
    Flash_WaitCycle(enFlashWait);                       // Flash ??????1?????????
    
    stcClkConfig.enClkSrc    = SysctrlClkXTH;           //????????????????????????,32M
    stcClkConfig.enHClkDiv   = SysctrlHclkDiv1;         // HCLK = SystemClk/1
    stcClkConfig.enPClkDiv   = SysctrlPclkDiv1;         // PCLK = HCLK/1
    Sysctrl_SetXTHFreq(SysctrlXthFreq20_32MHz);         //???????????????????????????20~32M
    Sysctrl_ClkInit(&stcClkConfig);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);   //GPIO ??????????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBTim, TRUE);   //Base Timer??????????????????
    
    stcTIM0Port.enDir  = GpioDirIn;
#if 0
    Gpio_Init(GpioPortA, GpioPin0, &stcTIM0Port);
    Gpio_SetAfMode(GpioPortA,GpioPin0,GpioAf7);            //PA00?????????TIM0_CHA
#endif
    Gpio_Init(GpioPortA, GpioPin1, &stcTIM0Port);
    Gpio_SetAfMode(GpioPortA,GpioPin1,GpioAf3);            //PA01?????????TIM0_CHB
        
    stcBtBaseCfg.enWorkMode    = BtWorkMode2;              //???????????????
    stcBtBaseCfg.enCT          = BtTimer;                  //???????????????????????????????????????PCLK
    stcBtBaseCfg.enPRS         = BtPCLKDiv64;              //PCLK/64
    stcBtBaseCfg.enCntDir      = BtCntUp;                  //??????????????????????????????????????????
    
    stcBtBaseCfg.pfnTim0Cb  = Tim0Int;                     //??????????????????
    
    Bt_Mode23_Init(TIM0, &stcBtBaseCfg);                   //TIM0 ?????????0???????????????
    
    stcBtPortCapCfg.enCh0BCmpCap   = BtCHxCapMode;         //CHB???????????????????????????
    stcBtPortCapCfg.enCH0BCapSel   = BtCHxCapFallRise;     //CHB???????????????????????????????????????
    stcBtPortCapCfg.enCH0BInFlt    = BtFltPCLKDiv16Cnt3;   //PCLK/16 3???????????????
    stcBtPortCapCfg.enCH0BPolarity = BtPortPositive;       //??????????????????
    
    Bt_M23_PortInput_Config(TIM0, &stcBtPortCapCfg);       //???????????????????????????
    
    u16ArrValue = 0xFFFF;
    Bt_M23_ARRSet(TIM0, u16ArrValue, TRUE);                //???????????????,???????????????
    
    u16CntValue = 0;
    Bt_M23_Cnt16Set(TIM0, u16CntValue);                    //??????????????????
    
    Bt_ClearAllIntFlag(TIM0);                              //???????????????
    Bt_Mode23_EnableIrq(TIM0,BtUevIrq);                    //??????TIM0 UEV????????????
    Bt_Mode23_EnableIrq(TIM0,BtCB0Irq);                    //??????TIM0 CB0??????/????????????
    EnableNvic(TIM0_IRQn, IrqLevel0, TRUE);                //TIM0????????????
    
    Bt_M23_Run(TIM0);                                      //TIM0 ??????

    while (1);

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900


