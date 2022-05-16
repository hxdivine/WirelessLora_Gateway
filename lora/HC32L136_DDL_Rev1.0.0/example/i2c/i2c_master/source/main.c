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
#include "i2c.h"
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
#define     T1_PORT                 (3)
#define     T1_PIN                  (3)

#define I2C_DEVADDR 0x50
#define READLEN   10
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
 //广播地址时，地址发送0x00，广播地址从机只接收，不回传
uint8_t u8Senddata[10] = {0x12,0x34,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x99};
uint8_t u8Recdata[10]={0x00};
uint8_t u8SendLen=0;
uint8_t u8RecvLen=0;
uint8_t SendFlg = 0,Comm_flg = 0;
uint8_t u8recvflg=0;
uint8_t u8State = 0;
uint8_t u8Addr = 0x00;//地址字节
int32_t main(void)
{ 
    stc_gpio_config_t stcGpioCfg;
    stc_i2c_config_t stcI2cCfg;
    stc_sysctrl_clk_config_t stcCfg;
    DDL_ZERO_STRUCT(stcCfg);
    DDL_ZERO_STRUCT(stcI2cCfg);
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_ClkSourceEnable(SysctrlClkRCL,TRUE);
    Sysctrl_SysClkSwitch(SysctrlClkRCL);
    Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);
	Sysctrl_SysClkSwitch(SysctrlClkRCH);
	Sysctrl_ClkSourceEnable(SysctrlClkRCL,FALSE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralI2c0,TRUE);
    
    stcGpioCfg.enDir = GpioDirOut;
    stcGpioCfg.enOD = GpioOdEnable;
    stcGpioCfg.enPuPd = GpioPu;
    
    Gpio_Init(GpioPortB,GpioPin8,&stcGpioCfg);
    Gpio_Init(GpioPortB,GpioPin9,&stcGpioCfg);
    
    Gpio_SetAfMode(GpioPortB,GpioPin8,GpioAf1);
    Gpio_SetAfMode(GpioPortB,GpioPin9,GpioAf1);
    
    stcI2cCfg.enFunc = I2cBaud_En;
    //stcI2cCfg.u8Tm = 0x04;//400kbps=(16000000/(8*(4+1))
    stcI2cCfg.u8Tm = 0x02;//1M=(24000000/(8*(2+1))
    stcI2cCfg.pfnI2c0Cb = NULL;
    stcI2cCfg.bTouchNvic = FALSE;
    
    I2C_DeInit(I2C0); 
    I2C_Init(I2C0,&stcI2cCfg);
    I2C_SetFunc(I2C0,I2cMode_En);
    I2C_SetFunc(I2C0,I2cHlm_En);
    I2C_SetFunc(I2C0,I2cStart_En);
	I2C_MasterWriteData(I2C0,I2C_DEVADDR<<1,u8Addr,u8Senddata,10);
	I2C_MasterReadData(I2C0,I2C_DEVADDR<<1,u8Recdata,u8Addr,10);
    while(1)
    {
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900


