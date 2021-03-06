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
void Tim0Int(void)
{
    static uint8_t i;
    
    if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
    {
        if(0 == i)
        {
            Gpio_WriteOutputIO(GpioPortD, GpioPin5, TRUE);
            i++;
        }
        else if(1 == i)
        {
            Gpio_WriteOutputIO(GpioPortD, GpioPin5, FALSE);
            i = 0;
        }
        
        Bt_ClearIntFlag(TIM0,BtUevIrq);
    }
}

void AdcJQRIrqCallback(void)
{
    static uint8_t i;
    
    Adc_GetJqrResult(&u16AdcResult02, 0);
    Adc_GetJqrResult(&u16AdcResult03, 1);
    Adc_GetJqrResult(&u16AdcResult05, 2);
    
    if(0 == i)
    {
        Bt_M23_CCR_Set(TIM0, BtCCR0A, 0x3000);
        i++;
    }
    else if(1 == i)
    {
        Bt_M23_CCR_Set(TIM0, BtCCR0A, 0x6000);
        i = 0;
    }
}

void ConfigAdc(void)
{
    uint8_t                    u8AdcScanCnt;
    stc_adc_cfg_t              stcAdcCfg;
    stc_adc_irq_t              stcAdcIrq;
    stc_adc_irq_calbakfn_pt_t  stcAdcIrqCalbaks;
    stc_adc_ext_trig_cfg_t     stcAdcExtTrigCfg;
    stc_gpio_config_t          stcAdcPort;

    
    
    DDL_ZERO_STRUCT(stcAdcCfg);
    DDL_ZERO_STRUCT(stcAdcIrq);
    DDL_ZERO_STRUCT(stcAdcIrqCalbaks);
    DDL_ZERO_STRUCT(stcAdcExtTrigCfg);
    DDL_ZERO_STRUCT(stcAdcPort);
    
        
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    stcAdcPort.enDir = GpioDirIn;
    
    Gpio_SetAnalogMode(GpioPortA, GpioPin2);        //PA02
    
    Gpio_Init(GpioPortA, GpioPin2, &stcAdcPort);

    Gpio_SetAnalogMode(GpioPortA, GpioPin3);        //PA03
    Gpio_Init(GpioPortA, GpioPin3, &stcAdcPort);
    
    Gpio_SetAnalogMode(GpioPortA, GpioPin5);        //PA05
    Gpio_Init(GpioPortA, GpioPin5, &stcAdcPort);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE);
    
    //ADC??????
    Adc_Enable();
    M0P_BGR->CR_f.BGR_EN = 0x1u;//BGR????????????
    M0P_BGR->CR_f.TS_EN  = 0x0u;
    delay100us(10);
    
    stcAdcCfg.enAdcOpMode      = AdcSCanMode;          //??????????????????
    stcAdcCfg.enAdcClkDiv      = AdcClkSysTDiv2;       //Adc???????????? PCLK/2
    stcAdcCfg.enAdcSampTimeSel = AdcSampTime6Clk;      //???????????? 6?????????
    stcAdcCfg.enAdcRefVolSel   = RefVolSelAVDD;        //??????AVDD
    stcAdcCfg.bAdcInBufEn      = FALSE;

    
    Adc_Init(&stcAdcCfg);                              //Adc?????????
    
    Adc_ConfigJqrChannel(JQRCH0MUX, AdcExInputCH2);    //??????????????????????????????
    Adc_ConfigJqrChannel(JQRCH1MUX, AdcExInputCH3);
    Adc_ConfigJqrChannel(JQRCH2MUX, AdcExInputCH5);
    
    stcAdcIrq.bAdcJQRIrq = TRUE;
    stcAdcIrqCalbaks.pfnAdcJQRIrq = AdcJQRIrqCallback;
    Adc_ConfigIrq(&stcAdcIrq, &stcAdcIrqCalbaks);       //????????????????????????
    
    u8AdcScanCnt = 3;                                   //????????????3???(3-1????????????????????????)
    
    Adc_ConfigJqrMode(&stcAdcCfg, u8AdcScanCnt, FALSE); //??????????????????????????????
    
    stcAdcExtTrigCfg.enAdcExtTrigRegSel = AdcExtTrig1;
    stcAdcExtTrigCfg.enAdcTrig1Sel      = AdcTrigTimer0;
    Adc_ExtTrigCfg(&stcAdcExtTrigCfg);                  //Timer0????????????????????????
    
    Adc_EnableIrq();                                    //??????Adc??????
    EnableNvic(ADC_IRQn, IrqLevel1, TRUE);              //Adc?????????
    
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
    uint16_t                      u16CompareAValue;
    uint16_t                      u16CntValue;
    uint8_t                       u8ValidPeriod;
    en_flash_waitcycle_t          enFlashWait;
    stc_sysctrl_clk_config_t      stcClkConfig;
    stc_bt_mode23_config_t        stcBtBaseCfg;
    stc_bt_m23_compare_config_t   stcBtPortCmpCfg;
    stc_bt_m23_adc_trig_config_t  stcBtTrigAdc;
    stc_bt_m23_dt_config_t        stcBtDeadTimeCfg;
    stc_gpio_config_t             stcTIM0Port;
    stc_gpio_config_t             stcLEDPort;
    
    DDL_ZERO_STRUCT(stcClkConfig);
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    DDL_ZERO_STRUCT(stcTIM0Port);
    DDL_ZERO_STRUCT(stcLEDPort);
    DDL_ZERO_STRUCT(stcBtPortCmpCfg);
    DDL_ZERO_STRUCT(stcBtTrigAdc);
    DDL_ZERO_STRUCT(stcBtDeadTimeCfg);
    
    enFlashWait = FlashWaitCycle1;                      //????????????????????????1??????HCLK??????24MHz??????????????????1???
    Flash_WaitCycle(enFlashWait);                       // Flash ??????1?????????
    
    stcClkConfig.enClkSrc    = SysctrlClkXTH;           //????????????????????????,32M
    stcClkConfig.enHClkDiv   = SysctrlHclkDiv1;         // HCLK = SystemClk/1
    stcClkConfig.enPClkDiv   = SysctrlPclkDiv1;         // PCLK = HCLK/1
    Sysctrl_SetXTHFreq(SysctrlXthFreq20_32MHz);         //???????????????????????????20~32M
    Sysctrl_ClkInit(&stcClkConfig);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);   //GPIO ??????????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBTim, TRUE);   //Base Timer??????????????????
        
    stcLEDPort.enDir  = GpioDirOut;
    Gpio_Init(GpioPortD, GpioPin5, &stcLEDPort);              //PD05?????????LED??????
    
    stcTIM0Port.enDir  = GpioDirOut;
    
    Gpio_Init(GpioPortA, GpioPin0, &stcTIM0Port);
    Gpio_SetAfMode(GpioPortA,GpioPin0,GpioAf7);            //PA00?????????TIM0_CHA
    
    Gpio_Init(GpioPortA, GpioPin1, &stcTIM0Port);
    Gpio_SetAfMode(GpioPortA,GpioPin1,GpioAf3);            //PA01?????????TIM0_CHB
        
    stcBtBaseCfg.enWorkMode    = BtWorkMode3;              //???????????????
    stcBtBaseCfg.enCT          = BtTimer;                  //???????????????????????????????????????PCLK
    stcBtBaseCfg.enPRS         = BtPCLKDiv1;               //PCLK
    //stcBtBaseCfg.enCntDir    = BtCntUp;                  //??????????????????????????????????????????
    stcBtBaseCfg.enPWMTypeSel  = BtComplementaryPWM;       //????????????PWM
    stcBtBaseCfg.enPWM2sSel    = BtSinglePointCmp;         //??????????????????
    stcBtBaseCfg.bOneShot      = FALSE;                    //????????????
    stcBtBaseCfg.bURSSel       = FALSE;                    //???????????????
    
    stcBtBaseCfg.pfnTim0Cb  = Tim0Int;                     //??????????????????
    
    Bt_Mode23_Init(TIM0, &stcBtBaseCfg);                   //TIM0 ?????????0???????????????
        
    u16ArrValue = 0x9000;
    Bt_M23_ARRSet(TIM0, u16ArrValue, TRUE);                //???????????????,???????????????
    
    u16CompareAValue = 0x6000;
    Bt_M23_CCR_Set(TIM0, BtCCR0A, u16CompareAValue);       //???????????????A,(PWM???????????????????????????????????????A)
    
    
    stcBtPortCmpCfg.enCH0ACmpCtrl   = BtPWMMode2;          //OCREFA????????????OCMA:PWM??????2
    stcBtPortCmpCfg.enCH0APolarity  = BtPortPositive;      //????????????
    stcBtPortCmpCfg.bCh0ACmpBufEn   = TRUE;                //A??????????????????
    stcBtPortCmpCfg.enCh0ACmpIntSel = BtCmpIntNone;        //A??????????????????:???
    
    stcBtPortCmpCfg.enCH0BCmpCtrl   = BtPWMMode2;          //OCREFB????????????OCMB:PWM??????2(PWM????????????????????????????????????????????????)
    stcBtPortCmpCfg.enCH0BPolarity  = BtPortPositive;      //????????????
    //stcBtPortCmpCfg.bCH0BCmpBufEn   = TRUE;              //B????????????????????????
    stcBtPortCmpCfg.enCH0BCmpIntSel = BtCmpIntNone;        //B??????????????????:???
    
    Bt_M23_PortOutput_Config(TIM0, &stcBtPortCmpCfg);      //????????????????????????
    
    stcBtTrigAdc.bEnTrigADC    = TRUE;                     //??????ADC??????????????????
    stcBtTrigAdc.bEnUevTrigADC = TRUE;                     //Uev????????????ADC
    Bt_M23_TrigADC_Config(TIM0, &stcBtTrigAdc);            //??????ADC??????
    
    u8ValidPeriod = 1;                                     //???????????????????????????0????????????????????????????????????????????????+1????????????????????????
    Bt_M23_SetValidPeriod(TIM0,u8ValidPeriod);             //??????????????????
    
    stcBtDeadTimeCfg.bEnDeadTime      = TRUE;
    stcBtDeadTimeCfg.u8DeadTimeValue  = 0xFF;
    Bt_M23_DT_Config(TIM0, &stcBtDeadTimeCfg);             //????????????
    
    ConfigAdc();
    
    u16CntValue = 0;
    
    Bt_M23_Cnt16Set(TIM0, u16CntValue);                    //??????????????????
    
    Bt_ClearAllIntFlag(TIM0);                              //???????????????
    Bt_Mode23_EnableIrq(TIM0,BtUevIrq);                    //??????TIM0 UEV????????????
    EnableNvic(TIM0_IRQn, IrqLevel0, TRUE);                //TIM0????????????   
    
    Bt_M23_EnPWM_Output(TIM0, TRUE, FALSE);                //TIM0 ??????????????????
    
    Bt_M23_Run(TIM0);                                      //TIM0 ?????????

    while (1);
    
    
    
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900


