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
 **   - 2016-02-16  1.0  XYZ First version for Device Driver Library of Module.
 **
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900

/******************************************************************************
 * Include files
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
#include "rtc.h"
#include "lpm.h"
#include "gpio.h"

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
#define     T1_PORT                 (0)
#define     T1_PIN                  (3)
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
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
uint8_t cyccnt=0;
uint8_t u8Alarmcnt=0;
uint8_t flg=0;
stc_rtc_time_t  stcReadTime;
static void RtcCycCb(void)
{
    cyccnt++;
    flg = ~flg;
    Gpio_WriteOutputIO(GpioPortD,GpioPin5,flg);
}
static void RtcAlarmCb(void)
{
  u8Alarmcnt++;
}
int32_t main(void)
{
    stc_rtc_config_t stcRtcConfig; 
    stc_rtc_irq_cb_t stcIrqCb;
    stc_rtc_time_t  stcTime;
    stc_rtc_alarmset_t stcAlarm;
    stc_rtc_cyc_sel_t   stcCycSel;
    stc_lpm_config_t stcLpmCfg;
    stc_gpio_config_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcRtcConfig);
    DDL_ZERO_STRUCT(stcIrqCb);
    DDL_ZERO_STRUCT(stcAlarm);
    DDL_ZERO_STRUCT(stcTime);
    DDL_ZERO_STRUCT(stcCycSel);
    DDL_ZERO_STRUCT(stcLpmCfg);
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    stcLpmCfg.enSEVONPEND   = SevPndDisable;
    stcLpmCfg.enSLEEPDEEP = SlpDpEnable;//SlpDpDisable;//
    stcLpmCfg.enSLEEPONEXIT = SlpExtEnable;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);  //GPIO外设时钟打开
    Sysctrl_SetPeripheralGate(SysctrlPeripheralRtc,TRUE);   //RTC模块时钟打开
    
    Sysctrl_ClkSourceEnable(SysctrlClkXTL,TRUE);
    
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(GpioPortD,GpioPin5,&stcGpioCfg);
    
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(GpioPortD,GpioPin4,&stcGpioCfg);
    
    stcRtcConfig.enClkSel = RtcClk32768;   //外部32.768k
    stcRtcConfig.enAmpmSel = Rtc24h;       //Rtc12h;//

    stcCycSel.enCyc_sel = RtcPradx;        //由PRDX产生周期中断
    stcCycSel.u8Prdx = 7u;                 //每隔4s产生一次周期中断          

    stcRtcConfig.pstcCycSel = &stcCycSel;
    
#if 0
    Rtc_DisableFunc(RtcCount);
    stcAlarm.u8Minute = 0x59;
    stcAlarm.u8Hour = 0x10;
    stcAlarm.u8Week = 0x02;
    Rtc_DisableFunc(RtcAlarmEn);
    Rtc_EnAlarmIrq(Rtc_AlarmInt_Enable);
    Rtc_SetAlarmTime(&stcAlarm);
    Rtc_EnableFunc(RtcAlarmEn);
#endif
    
    stcTime.u8Year = 0x18;
    stcTime.u8Month = 0x04;
    stcTime.u8Day = 0x16;
    stcTime.u8Hour = 0x10;
    stcTime.u8Minute = 0x58;
    stcTime.u8Second = 0x55;
    stcTime.u8DayOfWeek = Rtc_CalWeek(&stcTime.u8Day);
    stcRtcConfig.pstcTimeDate = &stcTime;
    
    stcIrqCb.pfnAlarmIrqCb = RtcAlarmCb;
    stcIrqCb.pfnTimerIrqCb = RtcCycCb;
    stcRtcConfig.pstcIrqCb = &stcIrqCb;
    stcRtcConfig.bTouchNvic = TRUE;
    
    Rtc_DisableFunc(RtcCount);
    Rtc_Init(&stcRtcConfig); 
    
    Rtc_EnableFunc(RtcCount);
    
#if 1
    
    //警告：
    //按下user按键(PD04)，则进入低功耗模式，否则在等待
    //注：若芯片处于低功耗模式，则芯片无法使用SWD进行调式和下载功能。
    //如需要再次下载程序，需要将芯片复位，且不按user按键
    while(1 == Gpio_GetInputIO(GpioPortD,GpioPin4));
    
    //不使用的引脚改为输出低
    M0P_GPIO->PAADS = 0;
    M0P_GPIO->PBADS = 0;
    M0P_GPIO->PDADS = 0;
    M0P_GPIO->PCADS = 0xC000;
    
    M0P_GPIO->PADIR = 0;
    M0P_GPIO->PBDIR = 0;
    M0P_GPIO->PCDIR = 0xC000;
    M0P_GPIO->PDDIR = 0x0010;
    
    M0P_GPIO->PAOUT = 0;
    M0P_GPIO->PBOUT = 0;
    M0P_GPIO->PCOUT = 0;
    M0P_GPIO->PDOUT = 0x0010;
    
    Lpm_Config(&stcLpmCfg);
    Lpm_GotoLpmMode();
#endif    
    while (1)
    {
        if(cyccnt>10)
        {
            cyccnt = 0;
            Rtc_ReadDateTime(&stcReadTime);
        }
    }

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900


