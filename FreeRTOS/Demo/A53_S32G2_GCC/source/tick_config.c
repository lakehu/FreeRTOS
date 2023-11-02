/*
 * Copyright 2022 NXP.
 */

#include <stdint.h>
#include "hw_system.h"
#include "generic_timer.h"
#include "gic.h"
#include "FreeRTOS.h"

#define PHYSICAL_TIMER_EL1_INT_ID    (30U)
#define STM0_INT_ID                  (56U)

extern void FreeRTOS_Tick_Handler(void);
void vConfigureTickInterrupt( void );
void vClearTickInterrupt(void);


/*
 * The application must provide a function that configures a peripheral to
 * create the FreeRTOS tick interrupt, then define configSETUP_TICK_INTERRUPT()
 * in FreeRTOSConfig.h to call the function.
 */
void vConfigureTickInterrupt( void )
{
    /* using stm as tick */
    STM_ENABLE(0, 0);
    STM_ENABLE(0, 0);
    STM_INT_CLEAR(0, 0); /* w1c interrupt */
    uint32_t stm0_cnt = STM_CNT_VAL(0);
    uint32_t stm0_cmp = stm0_cnt + (uint32_t)(configCPU_CLOCK_HZ/configTICK_RATE_HZ);
    STM_ENABLE_CHAN(0, 0, stm0_cmp);
    vGicRegisterIsrHandler(STM0_INT_ID, FreeRTOS_Tick_Handler);
    vGicSetIntPriority(STM0_INT_ID, portLOWEST_USABLE_INTERRUPT_PRIORITY);
    vGicEnableIntID(STM0_INT_ID);

    #if 0
    /* using Generic Timer as tick */
    uint64_t cnt_val = read_CNTPCT_EL0();
    cnt_val += (uint64_t)(configCPU_CLOCK_HZ/configTICK_RATE_HZ);
    write_CNTP_CVAL_EL0(cnt_val);
    vGicRegisterIsrHandler(PHYSICAL_TIMER_EL1_INT_ID, FreeRTOS_Tick_Handler);
    write_CNTP_CTL_EL0(CNTP_CTL_EL0_ENABLE_MASK & (~CNTP_CTL_EL0_IMASK_MASK));
    vGicSetIntPriority(PHYSICAL_TIMER_EL1_INT_ID,portLOWEST_USABLE_INTERRUPT_PRIORITY);
    vGicEnableIntID(PHYSICAL_TIMER_EL1_INT_ID);
    #endif
}

void vClearTickInterrupt(void)
{
    STM_INT_CLEAR(0, 0); /* w1c interrupt */
    STM_UPDATE_CHAN(0, 0, (uint32_t)(configCPU_CLOCK_HZ/configTICK_RATE_HZ));

    #if 0
    uint64_t cnt_val = read_CNTPCT_EL0();
    cnt_val += (uint64_t)(configCPU_CLOCK_HZ/configTICK_RATE_HZ);
    write_CNTP_CVAL_EL0(cnt_val);
    write_CNTP_CTL_EL0(CNTP_CTL_EL0_ENABLE_MASK & (~CNTP_CTL_EL0_IMASK_MASK));
    #endif
}

