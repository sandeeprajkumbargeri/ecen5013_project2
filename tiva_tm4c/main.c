//*****************************************************************************
// Author: Sandeep Raj Kumbargeri
// Date: 6-April 2018
//
// hw5 q2 - Simple example for creation of two tasks to blink different LEDs at
//          different frequencies in FreeRTOS using timer callback handler and
//          semaphores.
//
// Written for ECEN 5013 at University of Colorado Boulder in Spring 2018
//*****************************************************************************

#include "main.h"

void timer_callback(TimerHandle_t timer)
{
    BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
    static int count = 0;

    //For every two times, give the semaphore to 2hz task. Otherwise, give it  to 4hz task.
    if(count == 2)
    {
        xSemaphoreGiveFromISR(sem_bme280_acq, &pxHigherPriorityTaskWoken);
        count = -1;
    }
    else
        xSemaphoreGiveFromISR(sem_hcsr04_acq, &pxHigherPriorityTaskWoken);

    count++;
}

void UART0_Init(void)
{
    // Enable the peripherals used by this example.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

    // Set GPIO A0 and A1 as UART pins.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTStdioConfig(UART_PORT_0, UART_BAUD_RATE, g_ui32SysClock);
}


int main(void)
{
    BaseType_t timer_id = 0;
    TimerHandle_t timer;

    // Set the clocking to run directly from the crystal at 120MHz.
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), CLOCK_FREQUENCY);

    // Enable the GPIO port that is used for the on-board LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    // Check if the peripheral access is enabled.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));

    // Enable the GPIO pin for the LED (PN0).  Set the direction as output, and enable the GPIO pin for digital function.
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    comm_send_queue = xQueueCreate(100, SEND_PACKET_SIZE);
    comm_receive_queue = xQueueCreate(100, RECEIVE_PACKET_SIZE);

    comm_task_sem = xSemaphoreCreateCounting(SEMAPHORE_MAX_COUNT, SEMAPHORE_INITIAL_COUNT);
    sem_bme280_acq = xSemaphoreCreateBinary();
    sem_hcsr04_acq = xSemaphoreCreateBinary();
    sem_hcsr04_update = xSemaphoreCreateBinary();

    UART0_Init();

    // Create task for 2Hz Blinking LED
    xTaskCreate(Task_BME280, "Task dedicated to BME280 acquisitions", 1000, NULL, 1, NULL );

    // Create task for 2Hz Blinking LED
    xTaskCreate(Task_HCSR04, "Task dedicated to Ultrasonic Sensor acquisitions", 999, NULL, 1, NULL );

    // Create task for 2Hz Blinking LED
    xTaskCreate(vCommTask, "Task dedicated to UART connection", 1000, NULL, 1, NULL );

    //Create timer for 4hz and register the timer_callback handler
    timer = xTimerCreate("1Hz Timer", pdMS_TO_TICKS(TIMER_PERIOD), pdTRUE, (void *)&timer_id, timer_callback);
    xTimerStart(timer, portMAX_DELAY);

    // Start the scheduler
    vTaskStartScheduler();

    // Loop forever.
    while(1){
    }
}
