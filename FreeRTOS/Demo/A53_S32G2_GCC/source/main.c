/*
 * Copyright 2022 NXP.
 */

/******************************************************************************
 *
 * NOTE 1:  This project provides two demo applications.  A simple blinky
 * style project, and a more comprehensive test and demo application.  The
 * mainSELECTED_APPLICATION setting in main.c is used to select between the two.
 * See the notes on using mainSELECTED_APPLICATION where it is defined below.
 *
 * NOTE 2:  This file only contains the source code that is not specific to
 * either the simply blinky or full demos - this includes initialisation code
 * and callback functions.
 *
 */

#include <stdint.h>
#include "hw_system.h"
#include "gic.h"
#include "FreeRTOS.h"
#include "task.h"

/* mainSELECTED_APPLICATION is used to select between two demo applications,
 * as described at the top of this file.
 *
 * When mainSELECTED_APPLICATION is set to 0 the simple blinky example will
 * be run.
 *
 * When mainSELECTED_APPLICATION is set to 1 the comprehensive test and demo
 * application will be run.
 */
#define mainSELECTED_APPLICATION       0
/*-----------------------------------------------------------*/

#if ( mainSELECTED_APPLICATION == 0 )
    extern void main_blinky( void );
#elif ( mainSELECTED_APPLICATION == 1 )
    extern void main_regtest( void );
#endif

/*-----------------------------------------------------------*/

uint64_t ui32_ms_cnt = 0;
uint64_t ullErrorFound = pdFALSE;
/*-----------------------------------------------------------*/

int main(void)
{
    /* The mainSELECTED_APPLICATION setting is described at the top
    of this file. */
    #if( mainSELECTED_APPLICATION == 0 )
    {
        main_blinky();
    }
    #elif( mainSELECTED_APPLICATION == 1 )
    {
        main_regtest();
    }
    #endif

    /* Don't expect to reach here. */
    return 0;
}
/*-----------------------------------------------------------*/

void vApplicationTickHook(void)
{
    ui32_ms_cnt ++;
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
    /* Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeHeapSpace;

    /* This is just a trivial example of an idle hook.  It is called on each
    cycle of the idle task.  It must *NOT* attempt to block.  In this case the
    idle task just queries the amount of FreeRTOS heap that remains.  See the
    memory management section on the http://www.FreeRTOS.org web site for memory
    management options.  If there is a lot of heap memory free then the
    configTOTAL_HEAP_SIZE value in FreeRTOSConfig.h can be reduced to free up
    RAM. */
    xFreeHeapSpace = xPortGetFreeHeapSize();

    /* Remove compiler warning about xFreeHeapSpace being set but never used. */
    ( void ) xFreeHeapSpace;
}
/*-----------------------------------------------------------*/
