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
uint32_t u32PwcCapValue;
uint16_t u16TIM0_CntValue;

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
 * BT1??????????????????
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900
void Tim0Int(void)
{
    static uint16_t u16TIM0_OverFlowCnt;
    
    static uint16_t u16TIM0_CapValue;
     
    if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
    {
        u16TIM0_OverFlowCnt++;
        
        //u16TIM0_CntValue = 0;
        //u16TIM0_CapValue = 0;
        //u32PwcCapValue = 0;
        
        Bt_ClearIntFlag(TIM0,BtUevIrq);
    }

    if(TRUE == Bt_GetIntFlag(TIM0, BtCA0Irq))
    {
        u16TIM0_CntValue = Bt_M1_Cnt16Get(TIM0);
        u16TIM0_CapValue = Bt_M1_PWC_CapValueGet(TIM0);
        
        u32PwcCapValue = u16TIM0_OverFlowCnt*0x10000 + u16TIM0_CapValue;
        
        u16TIM0_OverFlowCnt = 0;
        
        Bt_ClearIntFlag(TIM0, BtCA0Irq);
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
    uint16_t                   u16CntValue;
    stc_bt_mode1_config_t      stcBtBaseCfg;
    stc_bt_pwc_input_config_t  stcBtPwcInCfg;
    stc_gpio_config_t          stcTIM0APort;
    stc_gpio_config_t          stcTIM0BPort;

    DDL_ZERO_STRUCT(stcBtBaseCfg);
    DDL_ZERO_STRUCT(stcTIM0APort);
    DDL_ZERO_STRUCT(stcTIM0BPort);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //????????????????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBTim, TRUE); //Base Timer??????????????????
    
    //PA00?????????TIM0_CHA
    //Gpio_ClrAnalogMode(GpioPortA, GpioPin0);
    stcTIM0APort.enDir  = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin0, &stcTIM0APort);
    Gpio_SetAfMode(GpioPortA,GpioPin0,GpioAf7);
    
    //PA01?????????TIM0_CHB
    //Gpio_ClrAnalogMode(GpioPortA, GpioPin1);
    //stcTIM0BPort.enDir  = GpioDirIn;
    //Gpio_Init(GpioPortA, GpioPin1, &stcTIM0BPort);
    //Gpio_SetAfMode(GpioPortA,GpioPin1,GpioAf3);
    
    stcBtBaseCfg.enWorkMode = BtWorkMode1;                  //???????????????
    stcBtBaseCfg.enCT       = BtTimer;                      //???????????????????????????????????????PCLK
    stcBtBaseCfg.enPRS      = BtPCLKDiv1;                   //PCLK
    stcBtBaseCfg.enOneShot  = BtPwcOneShotDetect;           //PWC????????????

    stcBtBaseCfg.pfnTim0Cb  = Tim0Int;                      //??????????????????
    
    Bt_Mode1_Init(TIM0, &stcBtBaseCfg);                     //TIM0 ?????????0???????????????
    
    stcBtPwcInCfg.enTsSel  = BtTs6IAFP;                     //PWC???????????? CHA
    stcBtPwcInCfg.enIA0Sel = BtIA0Input;                    //CHA??????IA0
    stcBtPwcInCfg.enFltIA0 = BtFltPCLKDiv16Cnt3;            //PCLK/16 3???????????????
    //stcBtPwcInCfg.enIB0Sel = BtIB0Input;                  //CHB??????IB0
    //stcBtPwcInCfg.enFltIB0 = BtFltPCLKDiv16Cnt3;          //PCLK/16 3???????????????
    
    Bt_M1_Input_Config(TIM0, &stcBtPwcInCfg);               //PWC????????????
    
    Bt_M1_PWC_Edge_Sel(TIM0, BtPwcRiseToFall);              //???????????????????????????
    
    u16CntValue = 0;
    
    Bt_M1_Cnt16Set(TIM0, u16CntValue);                      //??????????????????
		
    Bt_ClearIntFlag(TIM0,BtUevIrq);                         //???Uev????????????
    Bt_ClearIntFlag(TIM0,BtCA0Irq);                         //?????????????????????
    
    Bt_Mode1_EnableIrq(TIM0, BtUevIrq);                     //??????TIM0????????????
    Bt_Mode1_EnableIrq(TIM0, BtCA0Irq);                     //??????TIM0????????????
		
    EnableNvic(TIM0_IRQn, IrqLevel3, TRUE);                 //TIM0????????????
    
    Bt_M1_Run(TIM0);                                        //TIM0 ?????????
    
    while (1)
    {
        delay1ms(2000);
        Bt_M1_Run(TIM0);                                   //????????????PWC??????
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q??????164973950
//TEL???024-85718900


