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
#include "vc.h"
#include "gpio.h"
#include "adc.h"
#include "bt.h"
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
boolean_t bPortVal = 0;
void VcIrqCallback(void)
{
    //bPortVal = ~bPortVal;
    //Gpio_WriteOutputIO(GpioPortD,GpioPin5,bPortVal);
    Vc_ClearIrq(VcChannel0);
}

/*******************************************************************************
 * BT1??????????????????
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
void Tim0Int(void)
{
    static uint8_t i;
    
    if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
    {
        if(i%2 == 0)
        {
            Gpio_WriteOutputIO(GpioPortD, GpioPin5, TRUE);
        }
        else
        {
            Gpio_WriteOutputIO(GpioPortD, GpioPin5, FALSE);
        }
        
        i++;
        
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
void Vc_PortInit(void)
{
    stc_gpio_config_t stcGpioCfg;
    DDL_ZERO_STRUCT(stcGpioCfg);
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin6,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin6,GpioAf5);//VC??????
    Gpio_Init(GpioPortD,GpioPin5,&stcGpioCfg);
    Gpio_SetAnalogMode(GpioPortC,GpioPin0);//????????????
    //Gpio_SetAnalogMode(GpioPortA,GpioPin0);//????????????
}

int main(void)
{


    uint16_t                      u16ArrValue;
    uint16_t                      u16CompareAValue;
    uint16_t                      u16CompareBValue;
    uint16_t                      u16CntValue;
    uint8_t                       u8ValidPeriod;
    
    stc_vc_channel_config_t       stcChannelConfig;    
    stc_sysctrl_clk_config_t      stcClkConfig;
    stc_bt_mode23_config_t        stcBtBaseCfg;
    stc_bt_m23_compare_config_t   stcBtPortCmpCfg;
    stc_gpio_config_t             stcTIM0Port;
    stc_gpio_config_t             stcLEDPort;   
    stc_bt_m23_bk_input_config_t  stcBtBkInputCfg;
    
    en_flash_waitcycle_t          enFlashWait;
    
    DDL_ZERO_STRUCT(stcChannelConfig);//?????????0
    DDL_ZERO_STRUCT(stcClkConfig);
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    DDL_ZERO_STRUCT(stcBtPortCmpCfg);
    DDL_ZERO_STRUCT(stcTIM0Port);    
    DDL_ZERO_STRUCT(stcLEDPort);
    DDL_ZERO_STRUCT(stcBtBkInputCfg);   

    enFlashWait = FlashWaitCycle1;                      //????????????????????????1??????HCLK??????24MHz??????????????????1???
    Flash_WaitCycle(enFlashWait);                       // Flash ??????1?????????
    
    stcClkConfig.enClkSrc    = SysctrlClkXTH;           //????????????????????????,32M
    stcClkConfig.enHClkDiv   = SysctrlHclkDiv1;         // HCLK = SystemClk/1
    stcClkConfig.enPClkDiv   = SysctrlPclkDiv1;         // PCLK = HCLK/1
    Sysctrl_SetXTHFreq(SysctrlXthFreq20_32MHz);         //???????????????????????????20~32M
    Sysctrl_ClkInit(&stcClkConfig);
    
    
    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE))//???GPIO??????
    {
        return 1;
    }
    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralVcLvd, TRUE))//???LVD??????
    {
        return 1;
    } 
    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE))//???adc??????
    {
        return 1;
    }
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBTim, TRUE); //Base Timer??????????????????
    
    M0P_BGR->CR_f.BGR_EN = 0x1u;                 //BGR????????????
    M0P_BGR->CR_f.TS_EN = 0x0u;
    delay100us(1);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);   //GPIO ??????????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBTim, TRUE);   //Base Timer??????????????????
   
    ///< ????????????????????????->???????????????
    //stcLEDPort.enDrv = GpioDrvH;
    stcLEDPort.enDir  = GpioDirOut;
    Gpio_Init(GpioPortD, GpioPin5, &stcLEDPort);              //PD05?????????LED??????

    stcTIM0Port.enDir  = GpioDirOut;
    
    Gpio_Init(GpioPortA, GpioPin0, &stcTIM0Port);
    Gpio_SetAfMode(GpioPortA,GpioPin0,GpioAf7);            //PA00?????????TIM0_CHA
    
    Gpio_Init(GpioPortA, GpioPin1, &stcTIM0Port);
    Gpio_SetAfMode(GpioPortA,GpioPin1,GpioAf3);            //PA01?????????TIM0_CHB
        
    stcBtBaseCfg.enWorkMode    = BtWorkMode2;              //???????????????
    stcBtBaseCfg.enCT          = BtTimer;                  //???????????????????????????????????????PCLK
    stcBtBaseCfg.enPRS         = BtPCLKDiv1;               //PCLK
    stcBtBaseCfg.enCntDir      = BtCntUp;                  //??????????????????????????????????????????
    stcBtBaseCfg.enPWMTypeSel  = BtIndependentPWM;         //????????????PWM
    stcBtBaseCfg.enPWM2sSel    = BtSinglePointCmp;         //??????????????????
    stcBtBaseCfg.bOneShot      = FALSE;                    //????????????
    stcBtBaseCfg.bURSSel       = FALSE;                    //???????????????
    
    stcBtBaseCfg.pfnTim0Cb  = Tim0Int;                     //??????????????????
    
    Bt_Mode23_Init(TIM0, &stcBtBaseCfg);                   //TIM0 ?????????0???????????????
    
    Bt_ClearAllIntFlag(TIM0);                              //???????????????
   // EnableNvic(TIM0_IRQn, IrqLevel0, TRUE);                //TIM0????????????
    //Bt_Mode23_EnableIrq(TIM0,BtUevIrq);                    //??????TIM0 UEV????????????
    
    u16ArrValue = 0x9000;
    Bt_M23_ARRSet(TIM0, u16ArrValue, TRUE);                //???????????????,???????????????
    
    u16CompareAValue = 0x6000;
    Bt_M23_CCR_Set(TIM0, BtCCR0A, u16CompareAValue);       //???????????????A
    u16CompareBValue = 0x3000;
    Bt_M23_CCR_Set(TIM0, BtCCR0B, u16CompareBValue);       //???????????????B
    
    
    stcBtPortCmpCfg.enCH0ACmpCtrl   = BtPWMMode2;          //OCREFA????????????OCMA:PWM??????2
    stcBtPortCmpCfg.enCH0APolarity  = BtPortPositive;      //????????????
    stcBtPortCmpCfg.bCh0ACmpBufEn   = TRUE;                //A??????????????????
    stcBtPortCmpCfg.enCh0ACmpIntSel = BtCmpIntNone;        //A??????????????????:???
    
    stcBtPortCmpCfg.enCH0BCmpCtrl   = BtPWMMode2;          //OCREFB????????????OCMB:PWM??????2
    stcBtPortCmpCfg.enCH0BPolarity  = BtPortPositive;      //????????????
    stcBtPortCmpCfg.bCH0BCmpBufEn   = TRUE;                //B????????????????????????
    stcBtPortCmpCfg.enCH0BCmpIntSel = BtCmpIntNone;        //B??????????????????:???
    
    Bt_M23_PortOutput_Config(TIM0, &stcBtPortCmpCfg);      //????????????????????????
        
    u8ValidPeriod = 0;                                     //???????????????????????????0?????????????????????????????????????????????+1????????????1?????????
    Bt_M23_SetValidPeriod(TIM0,u8ValidPeriod);             //??????????????????
        
    u16CntValue = 0;
    
    Bt_M23_Cnt16Set(TIM0, u16CntValue);                    //??????????????????
    
    Bt_M23_EnPWM_Output(TIM0, TRUE, FALSE);                //TIM0 ??????????????????
    
    stcBtBkInputCfg.bEnBrake = TRUE;
    stcBtBkInputCfg.bEnVC0Brake = TRUE;
    stcBtBkInputCfg.bEnVC1Brake = FALSE;
    stcBtBkInputCfg.bEnSafetyBk = FALSE;
    stcBtBkInputCfg.bEnBKSync =   FALSE;
    stcBtBkInputCfg.enBkCH0AStat =BtCHxBksLow;
    stcBtBkInputCfg.enBkCH0BStat =BtCHxBksLow; 
    stcBtBkInputCfg.enBrakePolarity = BtPortPositive;
    stcBtBkInputCfg.enBrakeFlt = BtFltNone;
    Bt_M23_BrakeInput_Config(TIM0,&stcBtBkInputCfg);
    
    Bt_M23_Run(TIM0);                                      //TIM0 ??????   
    
    
    Vc_PortInit();                                        //????????????IO???
    
    stcChannelConfig.enVcChannel = VcChannel0;
    stcChannelConfig.enVcCmpDly  = VcDelay10mv;
    stcChannelConfig.enVcFilterTime = VcFilter28us;
    stcChannelConfig.enVcInPin_P = VcInPCh0;
    stcChannelConfig.enVcInPin_N = AiBg1p2;
    stcChannelConfig.enVcIrqSel = VcIrqHigh;
    stcChannelConfig.enVcOutConfig = VcOutTIMBK;          //?????????BT??????
    stcChannelConfig.pfnAnalogCmpCb = VcIrqCallback;
    Vc_EnableFilter(VcChannel0);
    Vc_ChannelInit(VcChannel0,&stcChannelConfig);
            
    
    Vc_EnableIrq(VcChannel0);
    Vc_EnableChannel(VcChannel0);
    
    while (1)
    {

    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900


