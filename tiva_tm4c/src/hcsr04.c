#include "../include/hcsr04.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"

#include <FreeRTOS.h>
#include "semphr.h"

extern SemaphoreHandle_t sem_hcsr04_acq, sem_hcsr04_update;
extern uint32_t sys_clock_get;
uint32_t pwm_width_1, pwm_width_2;

void TIMER0_Init(void)
{
    // Enable the Timer0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Wait for the Timer0 module to be ready.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

    // Configure TimerA as a half-width one-shot timer
    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);
}

void TIMER1_Init(void)
{
    // Enable the Timer0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    // Wait for the Timer0 module to be ready.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1));

    // Configure TimerA as a half-width one-shot timer
    TimerConfigure(TIMER1_BASE, TIMER_CFG_ONE_SHOT);
}

void PortLIntHandler(void)
{
    BaseType_t pxHigherPriorityTaskWoken = pdTRUE;

    if(GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_1) == GPIO_PIN_1)
    {
        //GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
        TimerLoadSet(TIMER0_BASE, TIMER_A, TIMER_A_HALF_WIDTH_MAX_VAL);
        TimerEnable(TIMER0_BASE, TIMER_A);
    }

    else if(GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_1) == 0)
    {
        //GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
        pwm_width_1 = TimerValueGet(TIMER0_BASE, TIMER_A);
        pwm_width_1 = TIMER_A_HALF_WIDTH_MAX_VAL - pwm_width_1;
        TimerDisable(TIMER0_BASE, TIMER_A);
        xSemaphoreGiveFromISR(sem_hcsr04_update, &pxHigherPriorityTaskWoken);
    }

    GPIOIntClear(GPIO_PORTL_BASE, GPIOIntStatus(GPIO_PORTL_BASE, true));
}

void PortHIntHandler(void)
{
    BaseType_t pxHigherPriorityTaskWoken = pdTRUE;

    if(GPIOPinRead(GPIO_PORTH_BASE, GPIO_PIN_1) == GPIO_PIN_1)
    {
        //GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
        TimerLoadSet(TIMER1_BASE, TIMER_A, TIMER_A_HALF_WIDTH_MAX_VAL);
        TimerEnable(TIMER1_BASE, TIMER_A);
    }

    else if(GPIOPinRead(GPIO_PORTH_BASE, GPIO_PIN_1) == 0)
    {
        //GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
        pwm_width_2 = TimerValueGet(TIMER1_BASE, TIMER_A);
        pwm_width_2 = TIMER_A_HALF_WIDTH_MAX_VAL - pwm_width_2;
        TimerDisable(TIMER1_BASE, TIMER_A);
        xSemaphoreGiveFromISR(sem_hcsr04_update, &pxHigherPriorityTaskWoken);
    }

    GPIOIntClear(GPIO_PORTH_BASE, GPIOIntStatus(GPIO_PORTH_BASE, true));
}

void HCSR04_1_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL));

    // Register the port-level interrupt handler. This handler is the first level interrupt handler for all the pin interrupts.
    GPIOIntRegister(GPIO_PORTL_BASE, PortLIntHandler);

    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_1);

    // Make pin 1 rising edge triggered interrupts.
    GPIOIntTypeSet(GPIO_PORTL_BASE, GPIO_PIN_1, GPIO_BOTH_EDGES);

    // Enable the pin interrupts.
    GPIOIntEnable(GPIO_PORTL_BASE, GPIO_PIN_1);
}

void HCSR04_2_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOH));

    // Register the port-level interrupt handler. This handler is the first level interrupt handler for all the pin interrupts.
    GPIOIntRegister(GPIO_PORTH_BASE, PortHIntHandler);

    GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOInput(GPIO_PORTH_BASE, GPIO_PIN_1);

    // Make pin 1 rising edge triggered interrupts.
    GPIOIntTypeSet(GPIO_PORTH_BASE, GPIO_PIN_1, GPIO_BOTH_EDGES);

    // Enable the pin interrupts.
    GPIOIntEnable(GPIO_PORTH_BASE, GPIO_PIN_1);
}

void Task_HCSR04(void *pvParameters)
{
    float usecs_1 = 0, usecs_2 = 0, cms_1 =  0, cms_2 = 0;
    char print_string[64];

    TIMER0_Init();
    TIMER1_Init();
    HCSR04_1_Init();
    HCSR04_2_Init();

    while(1)
    {
        xSemaphoreTake(sem_hcsr04_acq, portMAX_DELAY);

        GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0, GPIO_PIN_0);
            SysCtlDelay(sys_clock_get / 300000);
        GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0, 0);

        xSemaphoreTake(sem_hcsr04_update, portMAX_DELAY);
        usecs_1 = ((float) pwm_width_1 / (float) sys_clock_get) * 1000000;
        cms_1 = (float) usecs_1 / 58;

        GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_0, GPIO_PIN_0);
            SysCtlDelay(sys_clock_get / 300000);
        GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_0, 0);

        xSemaphoreTake(sem_hcsr04_update, portMAX_DELAY);
        usecs_2 = ((float) pwm_width_2 / (float) sys_clock_get) * 1000000;
        cms_2 = (float) usecs_2 / 58;

/*        sprintf(print_string, "D1: %f. D2: %f.\n", cms_1, cms_2);
        UARTprintf(print_string);*/
    }
}
