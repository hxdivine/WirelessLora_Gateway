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
#include "lcd.h"
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
void Lcd_SetPortAnalog(void)
{
    Gpio_SetAnalogMode(GpioPortA, GpioPin9);//COM0
    Gpio_SetAnalogMode(GpioPortA, GpioPin10);//COM1
    Gpio_SetAnalogMode(GpioPortA, GpioPin11);//COM2
    Gpio_SetAnalogMode(GpioPortA, GpioPin12);//COM3
    Gpio_SetAnalogMode(GpioPortA, GpioPin8);//SEG0
    Gpio_SetAnalogMode(GpioPortC, GpioPin9);//SEG1
    Gpio_SetAnalogMode(GpioPortC, GpioPin8);//SEG2
    Gpio_SetAnalogMode(GpioPortC, GpioPin7);//SEG3
    Gpio_SetAnalogMode(GpioPortC, GpioPin6);//SEG4
    Gpio_SetAnalogMode(GpioPortB, GpioPin15);//SEG5
    Gpio_SetAnalogMode(GpioPortB, GpioPin14);//SEG6
    Gpio_SetAnalogMode(GpioPortB, GpioPin13);//SEG7
    Gpio_SetAnalogMode(GpioPortB, GpioPin3);//VLCDH
    Gpio_SetAnalogMode(GpioPortB, GpioPin4);//VLCD3
    Gpio_SetAnalogMode(GpioPortB, GpioPin5);//VLCD2
    Gpio_SetAnalogMode(GpioPortB, GpioPin6);//VLCD1
}
int32_t main(void)
{
    uint8_t i;
    stc_lcd_config_t stcLcdCfg;
    stc_lcd_segcompara_t stcSegComPara;
    stc_lcd_segcom_t stcLcdSegCom;  
    
    DDL_ZERO_STRUCT(stcLcdCfg);
    DDL_ZERO_STRUCT(stcSegComPara);
    DDL_ZERO_STRUCT(stcLcdSegCom);
    
    Sysctrl_ClkSourceEnable(SysctrlClkRCL,TRUE);
    Sysctrl_SetRCLTrim(SysctrlRclFreq32768);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralLcd,TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
    Lcd_SetPortAnalog();//和LCD相关端口都要配置为模拟端口
    
    stcSegComPara.enBiasSrc = LcdExtCap;
    stcSegComPara.enDuty = LcdDuty4;
    stcSegComPara.u8MaxSeg = 8;
    LCD_GetSegCom(&stcSegComPara,&stcLcdSegCom);//根据DUTY及SRC获取COM端口配置
    for(i=0;i<stcSegComPara.u8MaxSeg;i++)
    {
        stcLcdSegCom.u32Seg0_31 |=1<<i;
    }
    stcLcdSegCom.u32Seg0_31 = ~stcLcdSegCom.u32Seg0_31;
    //超过32seg的由用户去设置stcLcdSegCom结构体中的seg32_39
    LCD_SetSegCom(&stcLcdSegCom);//comseg口配置
    stcLcdCfg.enDispMode = LcdMode0;
    stcLcdCfg.enBias = LcdBias3;
    stcLcdCfg.enBiasSrc = LcdInRes_High;
    stcLcdCfg.enClk = LcdRCL;
    stcLcdCfg.enDuty = LcdDuty4;
    stcLcdCfg.enCpClk = LcdClk2k;
    stcLcdCfg.enScanClk = LcdClk128hz;
    LCD_Init(&stcLcdCfg);
    LCD_EnFunc(LcdEn,TRUE);
    LCD_ClearDisp();
    LCD_WriteRam0_7Int32(0,0x0b060600);
    LCD_WriteRam0_7Int32(1,0x0d030f02);
    //LCD_FullDisp();
    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900


