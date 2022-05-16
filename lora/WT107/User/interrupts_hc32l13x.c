
#include "ddl.h"
#include "interrupts_hc32l13x.h"

/*******************************************************************************
 *                       IRQ WEAK DEFINE
 ******************************************************************************/
__weak void SysTick_IRQHandler(void);

/**
 *******************************************************************************
 ** \brief NVIC �ж�ʹ��
 **
 ** \param [in]  enIrq          �жϺ�ö������
 ** \param [in]  enLevel        �ж����ȼ�ö������
 ** \param [in]  bEn            �жϿ���
 ** \retval    Ok       ���óɹ�
 **            ����ֵ   ����ʧ��
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
 ** \brief NVIC hardware fault �ж�ʵ��
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
 ** \brief NVIC SysTick �ж�ʵ��
 **
 ** \retval
 ******************************************************************************/
void SysTick_Handler(void)
{
   SysTick_IRQHandler();
}
