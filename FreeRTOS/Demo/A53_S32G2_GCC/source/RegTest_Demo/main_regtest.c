/*
 * Copyright 2022 NXP.
 */

/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://aws.amazon.com/freertos
 *
 */


/*
 * In this demo using part of the FreeRTOS community demo main_full().
 * 
 * main_regtest() creates 2 tasks register tests then starts the scheduler.
 * 
 * "Reg test" tasks - These fill both the core and floating point registers with
 * known values, then check that each register maintains its expected value for
 * the lifetime of the task.  Each task uses a different set of values.  The reg
 * test tasks execute with a very low priority, so get preempted very
 * frequently.  A register containing an unexpected value is indicative of an
 * error in the context switching mechanism.
 *
 * "Check" task - The check task period is set to five seconds.  Each time it
 * executes it checks all the standard demo tasks, and the register check tasks,
 * are not only still executing, but are executing without reporting any errors,
 * then outputs the system status to the UART.
 */

#include <stdint.h>
#include "hw_system.h"
#include "FreeRTOS.h"
#include "task.h"

#define mainFLOP_TASK_PRIORITY               ( tskIDLE_PRIORITY )
#define mainCHECK_TASK_PRIORITY              ( configMAX_PRIORITIES - ( UBaseType_t ) 1 )

/* The period of the check task, in ms. */
#define mainNO_ERROR_CHECK_TASK_PERIOD       pdMS_TO_TICKS( ( TickType_t ) 200 )

/* Parameters that are passed into the register check tasks solely for the
purpose of ensuring parameters are passed into tasks correctly. */
#define mainREG_TEST_TASK_1_PARAMETER        ( ( void * ) 0x12345678 )
#define mainREG_TEST_TASK_2_PARAMETER        ( ( void * ) 0x87654321 )

/*
 * The check task, as described at the top of this file.
 */
static void prvCheckTask( void *pvParameters );

/*
 * Register check tasks, and the tasks used to write over and check the contents
 * of the FPU registers, as described at the top of this file.  The nature of
 * these files necessitates that they are written in an assembly file, but the
 * entry points are kept in the C file for the convenience of checking the task
 * parameter.
 */
static void prvRegTestTaskEntry1( void *pvParameters );
extern void vRegTest1Implementation( void );
static void prvRegTestTaskEntry2( void *pvParameters );
extern void vRegTest2Implementation( void );

/* The following two variables are used to communicate the status of the
register check tasks to the check task.  If the variables keep incrementing,
then the register check tasks have not discovered any errors.  If a variable
stops incrementing, then an error has been found. */
volatile uint64_t ullRegTest1LoopCounter = 0ULL, ullRegTest2LoopCounter = 0ULL;

extern uint64_t ullErrorFound;
/*-----------------------------------------------------------*/

void main_regtest(void)
{
    /* Create the register check tasks, as described at the top of this file */
    xTaskCreate( prvRegTestTaskEntry1, "Reg1", configMINIMAL_STACK_SIZE, mainREG_TEST_TASK_1_PARAMETER, tskIDLE_PRIORITY, NULL );
    xTaskCreate( prvRegTestTaskEntry2, "Reg2", configMINIMAL_STACK_SIZE, mainREG_TEST_TASK_2_PARAMETER, tskIDLE_PRIORITY, NULL );

    /* Create the task that performs the 'check' functionality,    as described at
    the top of this file. */
    xTaskCreate( prvCheckTask, "Check", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL );

    /* Start the scheduler. */
    vTaskStartScheduler();

    for( ;; );
}
/*-----------------------------------------------------------*/

static void prvCheckTask( void *pvParameters )
{
TickType_t xDelayPeriod = mainNO_ERROR_CHECK_TASK_PERIOD;
TickType_t xLastExecutionTime;
static uint64_t ullLastRegTest1Value = 0, ullLastRegTest2Value = 0;

    /* Just to stop compiler warnings. */
    ( void ) pvParameters;

    /* Initialise xLastExecutionTime so the first call to vTaskDelayUntil()
    works correctly. */
    xLastExecutionTime = xTaskGetTickCount();

    /* Cycle for ever, delaying then checking all the other tasks are still
    operating without error.  The system status is written to the UART on each
    iteration. */
    for( ;; )
    {
        /* Delay until it is time to execute again. */
        vTaskDelayUntil( &xLastExecutionTime, xDelayPeriod );

        /* Check that the register test 1 task is still running. */
        if( ullLastRegTest1Value == ullRegTest1LoopCounter )
        {
            ullErrorFound |= 1ULL << 17ULL;
        }
        ullLastRegTest1Value = ullRegTest1LoopCounter;

        /* Check that the register test 2 task is still running. */
        if( ullLastRegTest2Value == ullRegTest2LoopCounter )
        {
            ullErrorFound |= 1ULL << 18ULL;
        }
        ullLastRegTest2Value = ullRegTest2LoopCounter;

        configASSERT( ullErrorFound == pdFALSE );
    }
}
/*-----------------------------------------------------------*/

static void prvRegTestTaskEntry1( void *pvParameters )
{
    /* Although the regtest task is written in assembler, its entry point is
    written in C for convenience of checking the task parameter is being passed
    in correctly. */
    if( pvParameters == mainREG_TEST_TASK_1_PARAMETER )
    {
        /* The reg test task also tests the floating point registers.  Tasks
        that use the floating point unit must call vPortTaskUsesFPU() before
        any floating point instructions are executed. */
        vPortTaskUsesFPU();

        /* Start the part of the test that is written in assembler. */
        vRegTest1Implementation();
    }

    /* The following line will only execute if the task parameter is found to
    be incorrect.  The check task will detect that the regtest loop counter is
    not being incremented and flag an error. */
    vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/

static void prvRegTestTaskEntry2( void *pvParameters )
{
    /* Although the regtest task is written in assembler, its entry point is
    written in C for convenience of checking the task parameter is being passed
    in correctly. */
    if( pvParameters == mainREG_TEST_TASK_2_PARAMETER )
    {
        /* The reg test task also tests the floating point registers.  Tasks
        that use the floating point unit must call vPortTaskUsesFPU() before
        any floating point instructions are executed. */
        vPortTaskUsesFPU();

        /* Start the part of the test that is written in assembler. */
        vRegTest2Implementation();
    }

    /* The following line will only execute if the task parameter is found to
    be incorrect.  The check task will detect that the regtest loop counter is
    not being incremented and flag an error. */
    vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/
