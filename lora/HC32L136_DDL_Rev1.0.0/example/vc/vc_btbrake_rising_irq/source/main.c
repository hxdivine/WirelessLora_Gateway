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
//Q群：164973950
//TEL：024-85718900
/******************************************************************************/
//Q群：164973950
//TEL：024-85718900
/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2017-05-28 LiuHL    First Version
 **
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900

/******************************************************************************
 * Include files
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
#include "vc.h"
#include "gpio.h"
#include "adc.h"
#include "bt.h"
#include "flash.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900

/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
boolean_t bPortVal = 0;
void VcIrqCallback(void)
{
    //bPortVal = ~bPortVal;
    //Gpio_WriteOutputIO(GpioPortD,GpioPin5,bPortVal);
    Vc_ClearIrq(VcChannel0);
}

/*******************************************************************************
 * BT1中断服务函数
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
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
//Q群：164973950
//TEL：024-85718900
void Vc_PortInit(void)
{
    stc_gpio_config_t stcGpioCfg;
    DDL_ZERO_STRUCT(stcGpioCfg);
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin6,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin6,GpioAf5);//VC输出
    Gpio_Init(GpioPortD,GpioPin5,&stcGpioCfg);
    Gpio_SetAnalogMode(GpioPortC,GpioPin0);//模拟输入
    //Gpio_SetAnalogMode(GpioPortA,GpioPin0);//模拟输入
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
    
    DDL_ZERO_STRUCT(stcChannelConfig);//变量清0
    DDL_ZERO_STRUCT(stcClkConfig);
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    DDL_ZERO_STRUCT(stcBtPortCmpCfg);
    DDL_ZERO_STRUCT(stcTIM0Port);    
    DDL_ZERO_STRUCT(stcLEDPort);
    DDL_ZERO_STRUCT(stcBtBkInputCfg);   

    enFlashWait = FlashWaitCycle1;                      //读等待周期设置为1（当HCLK大于24MHz时必须至少为1）
    Flash_WaitCycle(enFlashWait);                       // Flash 等待1个周期
    
    stcClkConfig.enClkSrc    = SysctrlClkXTH;           //使用外部高速晶振,32M
    stcClkConfig.enHClkDiv   = SysctrlHclkDiv1;         // HCLK = SystemClk/1
    stcClkConfig.enPClkDiv   = SysctrlPclkDiv1;         // PCLK = HCLK/1
    Sysctrl_SetXTHFreq(SysctrlXthFreq20_32MHz);         //设置外部高速频率为20~32M
    Sysctrl_ClkInit(&stcClkConfig);
    
    
    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE))//开GPIO时钟
    {
        return 1;
    }
    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralVcLvd, TRUE))//开LVD时钟
    {
        return 1;
    } 
    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE))//开adc时钟
    {
        return 1;
    }
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBTim, TRUE); //Base Timer外设时钟使能
    
    M0P_BGR->CR_f.BGR_EN = 0x1u;                 //BGR必须使能
    M0P_BGR->CR_f.TS_EN = 0x0u;
    delay100us(1);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);   //GPIO 外设时钟使能
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBTim, TRUE);   //Base Timer外设时钟使能
   
    ///< 端口驱动能力配置->高驱动能力
    //stcLEDPort.enDrv = GpioDrvH;
    stcLEDPort.enDir  = GpioDirOut;
    Gpio_Init(GpioPortD, GpioPin5, &stcLEDPort);              //PD05设置为LED输出

    stcTIM0Port.enDir  = GpioDirOut;
    
    Gpio_Init(GpioPortA, GpioPin0, &stcTIM0Port);
    Gpio_SetAfMode(GpioPortA,GpioPin0,GpioAf7);            //PA00设置为TIM0_CHA
    
    Gpio_Init(GpioPortA, GpioPin1, &stcTIM0Port);
    Gpio_SetAfMode(GpioPortA,GpioPin1,GpioAf3);            //PA01设置为TIM0_CHB
        
    stcBtBaseCfg.enWorkMode    = BtWorkMode2;              //锯齿波模式
    stcBtBaseCfg.enCT          = BtTimer;                  //定时器功能，计数时钟为内部PCLK
    stcBtBaseCfg.enPRS         = BtPCLKDiv1;               //PCLK
    stcBtBaseCfg.enCntDir      = BtCntUp;                  //向上计数，在三角波模式时只读
    stcBtBaseCfg.enPWMTypeSel  = BtIndependentPWM;         //独立输出PWM
    stcBtBaseCfg.enPWM2sSel    = BtSinglePointCmp;         //单点比较功能
    stcBtBaseCfg.bOneShot      = FALSE;                    //循环计数
    stcBtBaseCfg.bURSSel       = FALSE;                    //上下溢更新
    
    stcBtBaseCfg.pfnTim0Cb  = Tim0Int;                     //中断函数入口
    
    Bt_Mode23_Init(TIM0, &stcBtBaseCfg);                   //TIM0 的模式0功能初始化
    
    Bt_ClearAllIntFlag(TIM0);                              //清中断标志
   // EnableNvic(TIM0_IRQn, IrqLevel0, TRUE);                //TIM0中断使能
    //Bt_Mode23_EnableIrq(TIM0,BtUevIrq);                    //使能TIM0 UEV更新中断
    
    u16ArrValue = 0x9000;
    Bt_M23_ARRSet(TIM0, u16ArrValue, TRUE);                //设置重载值,并使能缓存
    
    u16CompareAValue = 0x6000;
    Bt_M23_CCR_Set(TIM0, BtCCR0A, u16CompareAValue);       //设置比较值A
    u16CompareBValue = 0x3000;
    Bt_M23_CCR_Set(TIM0, BtCCR0B, u16CompareBValue);       //设置比较值B
    
    
    stcBtPortCmpCfg.enCH0ACmpCtrl   = BtPWMMode2;          //OCREFA输出控制OCMA:PWM模式2
    stcBtPortCmpCfg.enCH0APolarity  = BtPortPositive;      //正常输出
    stcBtPortCmpCfg.bCh0ACmpBufEn   = TRUE;                //A通道缓存控制
    stcBtPortCmpCfg.enCh0ACmpIntSel = BtCmpIntNone;        //A通道比较控制:无
    
    stcBtPortCmpCfg.enCH0BCmpCtrl   = BtPWMMode2;          //OCREFB输出控制OCMB:PWM模式2
    stcBtPortCmpCfg.enCH0BPolarity  = BtPortPositive;      //正常输出
    stcBtPortCmpCfg.bCH0BCmpBufEn   = TRUE;                //B通道缓存控制使能
    stcBtPortCmpCfg.enCH0BCmpIntSel = BtCmpIntNone;        //B通道比较控制:无
    
    Bt_M23_PortOutput_Config(TIM0, &stcBtPortCmpCfg);      //比较输出端口配置
        
    u8ValidPeriod = 0;                                     //事件更新周期设置，0表示锯齿波每个周期更新一次，每+1代表延迟1个周期
    Bt_M23_SetValidPeriod(TIM0,u8ValidPeriod);             //间隔周期设置
        
    u16CntValue = 0;
    
    Bt_M23_Cnt16Set(TIM0, u16CntValue);                    //设置计数初值
    
    Bt_M23_EnPWM_Output(TIM0, TRUE, FALSE);                //TIM0 端口输出使能
    
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
    
    Bt_M23_Run(TIM0);                                      //TIM0 运行   
    
    
    Vc_PortInit();                                        //配置测试IO口
    
    stcChannelConfig.enVcChannel = VcChannel0;
    stcChannelConfig.enVcCmpDly  = VcDelay10mv;
    stcChannelConfig.enVcFilterTime = VcFilter28us;
    stcChannelConfig.enVcInPin_P = VcInPCh0;
    stcChannelConfig.enVcInPin_N = AiBg1p2;
    stcChannelConfig.enVcIrqSel = VcIrqHigh;
    stcChannelConfig.enVcOutConfig = VcOutTIMBK;          //输出到BT刹车
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
//Q群：164973950
//TEL：024-85718900


