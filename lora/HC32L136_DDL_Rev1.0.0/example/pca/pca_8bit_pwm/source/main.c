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
#include "pca.h"
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

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
static volatile uint32_t u32PcaTestFlag = 0;
static volatile uint16_t u16CcapData[8] = {0};
static uint8_t  u8CmpHData;

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900


/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
/*******************************************************************************
 * Pca中断服务程序
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900
void PcaInt(void)
{
    if (TRUE == Pca_GetCntIntFlag())
    {
        Pca_ClearCntIntFlag();
        u32PcaTestFlag |= 0x20;
    }
    if (TRUE == Pca_GetIntFlag(Module0))
    {
        Pca_ClearIntFlag(Module0);
        u32PcaTestFlag |= 0x01;
    }
    if (TRUE == Pca_GetIntFlag(Module1))
    {
        Pca_ClearIntFlag(Module1);
        u32PcaTestFlag |= 0x02;
    }
    if (TRUE == Pca_GetIntFlag(Module2))
    {
        Pca_ClearIntFlag(Module2);
        u32PcaTestFlag |= 0x04;
    }
    if (TRUE == Pca_GetIntFlag(Module3))
    {
        Pca_ClearIntFlag(Module3);
        u32PcaTestFlag |= 0x08;
    }
    if (TRUE == Pca_GetIntFlag(Module4))
    {
        Pca_ClearIntFlag(Module4);
        u32PcaTestFlag |= 0x10;
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

int32_t main(void)
{
    stc_pca_config_t        stcConfig;
    stc_pca_capmodconfig_t  stcModConfig;
    stc_gpio_config_t       stcPCA0Port;
    uint8_t                 u8CmpLData = 0x20;
    
    u8CmpHData = 0x90;

    DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcModConfig);
    DDL_ZERO_STRUCT(stcPCA0Port);
    
    //PCA、GPIO外设时钟开启
    Sysctrl_SetPeripheralGate(SysctrlPeripheralPca, TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    //INT ENABLE
    //EnableNvic(PCA_IRQn, 3, TRUE);
    //Pca_EnableIrq(Module0);

    //GPIO CFG
    //PA06设置为PCA_CH0
    //Gpio_ClrAnalogMode(GpioPortA, GpioPin6);
    stcPCA0Port.enDrv  = GpioDrvH;
    stcPCA0Port.enDir  = GpioDirOut;
    Gpio_Init(GpioPortA, GpioPin6, &stcPCA0Port);
    Gpio_SetAfMode(GpioPortA, GpioPin6, GpioAf2);
    
    stcConfig.enCIDL = IdleGoOn;                   //休眠模式PCA工作 
    stcConfig.enWDTE = PCAWDTDisable;              //wdt功能关闭
    stcConfig.enCPS  = PCAPCLKDiv32;               //PCLK/32
    
    stcConfig.pfnPcaCb = PcaInt;                   //中断函数入口
    
    stcModConfig.enECOM     = ECOMEnable;          //比较器功能允许
    stcModConfig.enCAPP     = CAPPDisable;         //上升沿捕获禁止
    stcModConfig.enCAPN     = CAPNDisable;         //下降沿捕获禁止
    stcModConfig.enMAT      = MATDisable;          //禁止匹配
    stcModConfig.enTOG      = TOGDisable;          //禁止翻转
    stcModConfig.en8bitPWM  = PCA8bitPWMEnable;    //允许8位PWM功能
    
    
    if (Ok != Pca_Init(&stcConfig))
    {
        return Error;
    }
    if (Ok != Pca_CapModConfig(Module0, &stcModConfig))
    {
        return Error;
    }
    
    Pca_CmpDataLSet(Module0, u8CmpLData);        //比较寄存器初始化
    Pca_CmpDataHSet(Module0, u8CmpHData);        //(比较重载值)比较寄存器设置
    Pca_Run();
    
    while (1)
    {
        Pca_CmpDataHSet(Module0, u8CmpHData);    //(比较重载值)比较寄存器设置
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
//Q群：164973950
//TEL：024-85718900


