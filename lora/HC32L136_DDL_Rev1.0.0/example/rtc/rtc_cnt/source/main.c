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
 **   - 2018-05-03  1.0  CJ First version for Device Driver Library of Module.
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
}
static void RtcAlarmCb(void)
{
  u8Alarmcnt++;
}
uint8_t u8AmPm = 0;
int32_t main(void)
{
    
    stc_rtc_config_t stcRtcConfig; 
    stc_rtc_irq_cb_t stcIrqCb;
    stc_rtc_time_t  stcTime;
    stc_rtc_alarmset_t stcAlarm;
    stc_rtc_cyc_sel_t   stcCycSel;
    
    DDL_ZERO_STRUCT(stcRtcConfig);
    DDL_ZERO_STRUCT(stcIrqCb);
    DDL_ZERO_STRUCT(stcAlarm);
    DDL_ZERO_STRUCT(stcTime);
    DDL_ZERO_STRUCT(stcCycSel);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);//GPIO外设时钟打开
    Sysctrl_SetPeripheralGate(SysctrlPeripheralRtc,TRUE);//RTC模块时钟打开
    Sysctrl_ClkSourceEnable(SysctrlClkXTL,TRUE);
    
    stcRtcConfig.enClkSel = RtcClk32768;//RtcClkHxt1024;//RtcClk32;//
    stcRtcConfig.enAmpmSel = Rtc12h;//Rtc12h;//
      
    stcTime.u8Year = 0x17;
    stcTime.u8Month = 0x06;
    stcTime.u8Day = 0x07;
    stcTime.u8Hour = 0x12;
    stcTime.u8Minute = 0x59;
    stcTime.u8Second = 0x59;
    stcTime.u8DayOfWeek = Rtc_CalWeek(&stcTime.u8Day);
    stcRtcConfig.pstcTimeDate = &stcTime;
    
    stcIrqCb.pfnAlarmIrqCb = RtcAlarmCb;
    stcIrqCb.pfnTimerIrqCb = RtcCycCb;
    stcRtcConfig.pstcIrqCb = &stcIrqCb;
    stcRtcConfig.bTouchNvic = FALSE;
    
    Rtc_DisableFunc(RtcCount);
    Rtc_Init(&stcRtcConfig); 
    Rtc_EnableFunc(RtcCount);   
    while (1)
    {
      Rtc_ReadDateTime(&stcReadTime);
      if(0 == Rtc_GetHourMode())
        u8AmPm = Rtc_RDAmPm();
    }

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900


