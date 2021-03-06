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
#include "timer3.h"
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
uint16_t u16AdcResult02;
uint16_t u16AdcResult03;
uint16_t u16AdcResult05;

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
void Tim3Int(void)
{
    static uint8_t i;
    
    if(TRUE == Tim3_GetIntFlag(Tim3UevIrq))
    {
        if(0 == i)
        {
            Gpio_WriteOutputIO(GpioPortD, GpioPin5,TRUE);
            
            Tim3_M23_CCR_Set(Tim3CCR0A, 0x3000);
            Tim3_M23_CCR_Set(Tim3CCR0B, 0x6000);
            
            Tim3_M23_CCR_Set(Tim3CCR1A, 0x3000);
            Tim3_M23_CCR_Set(Tim3CCR1B, 0x6000);
            
            Tim3_M23_CCR_Set(Tim3CCR2A, 0x3000);
            Tim3_M23_CCR_Set(Tim3CCR2B, 0x6000);
            
            i++;
        }
        else if(1 == i)
        {
            Gpio_WriteOutputIO(GpioPortD, GpioPin5,FALSE);
            
            Tim3_M23_CCR_Set(Tim3CCR0A, 0x6000);
            Tim3_M23_CCR_Set(Tim3CCR0B, 0x3000);
            
            Tim3_M23_CCR_Set(Tim3CCR1A, 0x6000);
            Tim3_M23_CCR_Set(Tim3CCR1B, 0x3000);
            
            Tim3_M23_CCR_Set(Tim3CCR2A, 0x6000);
            Tim3_M23_CCR_Set(Tim3CCR2B, 0x3000);
            
            i = 0;
        }
        
        Tim3_ClearIntFlag(Tim3UevIrq);
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
    uint16_t                        u16ArrValue;
    uint16_t                        u16CompareAValue;
    uint16_t                        u16CompareBValue;
    uint16_t                        u16CntValue;
    uint8_t                         u8ValidPeriod;
    en_flash_waitcycle_t            enFlashWait;
    stc_sysctrl_clk_config_t        stcClkConfig;
    stc_tim3_mode23_config_t        stcTim3BaseCfg;
    stc_tim3_m23_compare_config_t   stcTim3PortCmpCfg;
    stc_gpio_config_t               stcTIM3Port;
    stc_gpio_config_t               stcLEDPort;
    
    DDL_ZERO_STRUCT(stcClkConfig);                      //????????????????????????
    DDL_ZERO_STRUCT(stcTim3BaseCfg);
    DDL_ZERO_STRUCT(stcTIM3Port);
    DDL_ZERO_STRUCT(stcLEDPort);
    DDL_ZERO_STRUCT(stcTim3PortCmpCfg);
    
    enFlashWait = FlashWaitCycle1;                      //????????????????????????1??????HCLK??????24MHz??????????????????1???
    Flash_WaitCycle(enFlashWait);                       // Flash ??????1?????????
    
    stcClkConfig.enClkSrc    = SysctrlClkXTH;           //????????????????????????,32M
    stcClkConfig.enHClkDiv   = SysctrlHclkDiv1;         // HCLK = SystemClk/1
    stcClkConfig.enPClkDiv   = SysctrlPclkDiv1;         // PCLK = HCLK/1
    Sysctrl_SetXTHFreq(SysctrlXthFreq20_32MHz);         //???????????????????????????20~32M
    Sysctrl_ClkInit(&stcClkConfig);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);   //GPIO ??????????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralTim3, TRUE);   //Timer3??????????????????
     
    stcLEDPort.enDir  = GpioDirOut;
    Gpio_Init(GpioPortD, GpioPin5, &stcLEDPort);              //PD05?????????LED??????
    
    stcTIM3Port.enDir  = GpioDirOut;
    
    Gpio_Init(GpioPortA, GpioPin8, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortA,GpioPin8,GpioAf2);               //PA08?????????TIM3_CH0A
    
    Gpio_Init(GpioPortA, GpioPin7, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortA,GpioPin7,GpioAf4);               //PA07?????????TIM3_CH0B
    
    Gpio_Init(GpioPortB, GpioPin10, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortB,GpioPin10,GpioAf5);              //PB10?????????TIM3_CH1A
    
    Gpio_Init(GpioPortB, GpioPin0, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortB,GpioPin0,GpioAf2);               //PB00?????????TIM3_CH1B
    
    Gpio_Init(GpioPortB, GpioPin8, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortB,GpioPin8,GpioAf6);               //PB08?????????TIM3_CH2A
    
    Gpio_Init(GpioPortB, GpioPin15, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortB,GpioPin15,GpioAf2);              //PB15?????????TIM3_CH2B
        
    stcTim3BaseCfg.enWorkMode    = Tim3WorkMode3;             //???????????????
    stcTim3BaseCfg.enCT          = Tim3Timer;                 //???????????????????????????????????????PCLK
    stcTim3BaseCfg.enPRS         = Tim3PCLKDiv1;              //PCLK
    //stcTim3BaseCfg.enCntDir    = Tim3CntUp;                 //??????????????????????????????????????????
    stcTim3BaseCfg.enPWMTypeSel  = Tim3IndependentPWM;        //????????????PWM    //Tim3ComplementaryPWM;
    stcTim3BaseCfg.enPWM2sSel    = Tim3DoublePointCmp;        //??????????????????
    stcTim3BaseCfg.bOneShot      = FALSE;                     //????????????
    stcTim3BaseCfg.bURSSel       = FALSE;                     //???????????????
    
    stcTim3BaseCfg.pfnTim3Cb  = Tim3Int;                      //??????????????????
    
    Tim3_Mode23_Init(&stcTim3BaseCfg);                        //TIM3 ?????????0???????????????
    
    u16ArrValue = 0x9000;
    Tim3_M23_ARRSet(u16ArrValue, TRUE);                       //???????????????,???????????????
    
    u16CompareAValue = 0x6000;
    u16CompareBValue = 0x3000;
    Tim3_M23_CCR_Set(Tim3CCR0A, u16CompareAValue);            //???????????????A,(PWM???????????????????????????????????????A)
    Tim3_M23_CCR_Set(Tim3CCR0B, u16CompareBValue);
    
    Tim3_M23_CCR_Set(Tim3CCR1A, u16CompareAValue);
    Tim3_M23_CCR_Set(Tim3CCR1B, u16CompareBValue);
    
    Tim3_M23_CCR_Set(Tim3CCR2A, u16CompareAValue);
    Tim3_M23_CCR_Set(Tim3CCR2B, u16CompareBValue);
    
    stcTim3PortCmpCfg.enCHxACmpCtrl   = Tim3PWMMode2;         //OCREFA????????????OCMA:PWM??????2
    stcTim3PortCmpCfg.enCHxAPolarity  = Tim3PortPositive;     //????????????
    stcTim3PortCmpCfg.bCHxACmpBufEn   = TRUE;                 //A??????????????????
    stcTim3PortCmpCfg.enCHxACmpIntSel = Tim3CmpIntNone;       //A??????????????????:???
    
    stcTim3PortCmpCfg.enCHxBCmpCtrl   = Tim3PWMMode2;         //OCREFB????????????OCMB:PWM??????2(PWM????????????????????????????????????????????????)
    stcTim3PortCmpCfg.enCHxBPolarity  = Tim3PortPositive;     //????????????
    stcTim3PortCmpCfg.bCHxBCmpBufEn   = TRUE;                 //B????????????????????????
    stcTim3PortCmpCfg.enCHxBCmpIntSel = Tim3CmpIntNone;       //B??????????????????:???
    
    Tim3_M23_PortOutput_Config(Tim3CH0, &stcTim3PortCmpCfg);  //????????????????????????
    Tim3_M23_PortOutput_Config(Tim3CH1, &stcTim3PortCmpCfg);  //????????????????????????
    Tim3_M23_PortOutput_Config(Tim3CH2, &stcTim3PortCmpCfg);  //????????????????????????
    
    
    u8ValidPeriod = 1;                                        //???????????????????????????0????????????????????????????????????????????????+1????????????????????????
    Tim3_M23_SetValidPeriod(u8ValidPeriod);                   //??????????????????
    
    u16CntValue = 0;
    
    Tim3_M23_Cnt16Set(u16CntValue);                          //??????????????????
    
    Tim3_ClearAllIntFlag();                                   //???????????????
    Tim3_Mode23_EnableIrq(Tim3UevIrq);                        //??????TIM3 UEV????????????
    EnableNvic(TIM3_IRQn, IrqLevel0, TRUE);                   //TIM3????????????
    
    Tim3_M23_EnPWM_Output(TRUE, FALSE);                      //??????????????????
    
    Tim3_M23_Run();                                          //?????????
    
    while (1);
    
    
    
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900


