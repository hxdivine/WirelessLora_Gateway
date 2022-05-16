
#include "ddl.h"
#include "interrupts_hc32l13x.h"

/*******************************************************************************
 *                       IRQ WEAK DEFINE
 ******************************************************************************/
__weak void SysTick_IRQHandler(void);

/**
 *******************************************************************************
 ** \brief NVIC 中断使能
 **
 ** \param [in]  enIrq          中断号枚举类型
 ** \param [in]  enLevel        中断优先级枚举类型
 ** \param [in]  bEn            中断开关
 ** \retval    Ok       设置成功
 **            其他值   设置失败
 ******************************************************************************/
void EnableNvic(IRQn_Type enIrq, en_irq_level_t enLevel, boolean_t bEn)
{
    NVIC_ClearPendingIRQ(enIrq);
    NVIC_SetPriority(enIrq, enLevel);
    if (TRUE == bEn)
    {
        NVIC_EnableIRQ(enIrq);
    }
    else
    {
        NVIC_DisableIRQ(enIrq);
    }
}

/**
 *******************************************************************************
 ** \brief NVIC hardware fault 中断实现
 **        
 **
 ** \retval
 ******************************************************************************/
void HardFault_Handler(void)
{    
    volatile int a = 0;

    while( 0 == a)
    {
        ;
    }
    
}


/**
 *******************************************************************************
 ** \brief NVIC SysTick 中断实现
 **
 ** \retval
 ******************************************************************************/
void SysTick_Handler(void)
{
   SysTick_IRQHandler();
}
