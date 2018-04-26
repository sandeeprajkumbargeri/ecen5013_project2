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

//void Task_BME280(void *pvParameters)
//{
//    BaseType_t led_status = OFF;
//
//    sem_bme280_acq = xSemaphoreCreateCounting(1, 0);
//    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
//
//    while(1)
//    {
//        xSemaphoreTake(sem_4hz, portMAX_DELAY);
//
//        if(led_status)
//        {
//            //Switch off LED
//            led_status = OFF;
//            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
//        }
//
//        else
//        {
//            //Switch on LED
//            led_status = ON;
//            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
//        }
//    }
//}


//void timer_callback(TimerHandle_t timer)
//{
//    BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
//    static int count = 0;
//
//    //For every two times, give the semaphore to 2hz task. Otherwise, give it  to 4hz task.
//    if(count == 2)
//    {
//        xSemaphoreGiveFromISR(sem_2hz, &pxHigherPriorityTaskWoken);
//        count = -1;
//    }
//    else
//        xSemaphoreGiveFromISR(sem_4hz, &pxHigherPriorityTaskWoken);
//
//    count++;
//}


int main(void)
{
    uint8_t received = 0;
    BaseType_t timer_id = 0;
    TimerHandle_t timer;
    BME280_config_t bme280_config;
    BME280_data_t bme280_compensated_data;

    // Set the clocking to run directly from the crystal at 120MHz.
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), CLOCK_FREQUENCY);

    // Enable the GPIO port that is used for the on-board LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    // Check if the peripheral access is enabled.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));

    // Enable the GPIO pin for the LED (PN0).  Set the direction as output, and enable the GPIO pin for digital function.
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    I2C0_Init();
    if(BME280_Init() == false)
        exit(1);

    bme280_config.ctrl_hum = BME280_CTRL_HUM_VAL_OVSP_X16;
    bme280_config.ctrl_meas = BME280_CTRL_MEAS_VAL_TEMP_OVSP_X16 | BME280_CTRL_MEAS_VAL_PRESS_OVSP_X16 | BME280_CTRL_MEAS_VAL_MODE_NORMAL;
    bme280_config.config = BME280_CONFIG_VAL_T_STANDBY_1000MS | BME280_CONFIG_VAL_FILTER_16;

    BME280_Configure(bme280_config);

    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1); //just for setting breakpoint and checking the value of received.

    bme280_compensated_data = BME280_GetCompensatedData();

    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1); //just for setting breakpoint and checking the value of received.

//    // Create task for 2Hz Blinking LED
//    xTaskCreate(Task_BME280, "Task dedicated to BME280 acquisitions", 1000, NULL, 1, NULL );
//
//    //Create timer for 4hz and register the timer_callback handler
//    timer = xTimerCreate("4Hz Timer", pdMS_TO_TICKS(TIMER_PERIOD), pdTRUE, (void *)&timer_id, timer_callback);
//    xTimerStart(timer, portMAX_DELAY);
//
//    // Start the scheduler
//    vTaskStartScheduler();
//
//    // Loop forever.
//    while(1){
//    }
}
