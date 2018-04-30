#include "include/comm_task.h"
#include "include/bme280.h"
#include "include/hcsr04.h"
#include "main.h"


#define SYS_CLOCK_MHZ           1000000
#define SYS_CLOCK_FREQUENCY     119*SYS_CLOCK_MHZ

/**
 * main.c
 */
void timer_callback(TimerHandle_t timer)
{
    BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
    static int count = 0;

    //For every two times, give the semaphore to 2hz task. Otherwise, give it  to 4hz task.
    if(count == 2)
    {
        BME280_task_events |= BME280_TASK_SEND_EVENT;
        xSemaphoreGiveFromISR(sem_bme280_acq, &pxHigherPriorityTaskWoken);
        count = -1;
    }
    else
    {
//        HCSR04_task_events |= HCSR04_TASK_ACQ_EVENT;

        xSemaphoreGiveFromISR(sem_hcsr04_acq, &pxHigherPriorityTaskWoken);
    }

    count++;
}

void main(void)
{

    BaseType_t timer_id = 0;
    TimerHandle_t timer;

    sys_clock_get = SysCtlClockFreqSet(SYSCTL_XTAL_25MHZ|SYSCTL_OSC_MAIN|SYSCTL_USE_PLL|SYSCTL_CFG_VCO_480, SYS_CLOCK_FREQUENCY);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));

    UART_Configure(sys_clock_get);

    comm_send_queue = xQueueCreate(100, SEND_PACKET_SIZE);
    comm_receive_queue = xQueueCreate(100, RECEIVE_PACKET_SIZE);
    bme280_queue = xQueueCreate(100, RECEIVE_PACKET_SIZE);
    hcsr04_queue = xQueueCreate(100, RECEIVE_PACKET_SIZE);

    comm_task_sem = xSemaphoreCreateCounting(SEMAPHORE_MAX_COUNT, SEMAPHORE_INITIAL_COUNT);
    sem_bme280_acq = xSemaphoreCreateCounting(SEMAPHORE_MAX_COUNT, SEMAPHORE_INITIAL_COUNT);
    sem_hcsr04_acq = xSemaphoreCreateCounting(SEMAPHORE_MAX_COUNT, SEMAPHORE_INITIAL_COUNT);
    sem_hcsr04_update = xSemaphoreCreateCounting(SEMAPHORE_MAX_COUNT, SEMAPHORE_INITIAL_COUNT);
    sem_hcsr04_cmd = xSemaphoreCreateCounting(SEMAPHORE_MAX_COUNT, SEMAPHORE_INITIAL_COUNT);

    /* Create COMM_TASK*/
    xTaskCreate( vCommTask, "COMM TASK", 1000, NULL, 1, NULL ); /* This COMM_TASK does not use the task handle. */

    /* Create Motion Detection Task*/
    xTaskCreate( Task_HCSR04, "Motion Detection Task", 1000, NULL, 1, NULL ); /* This COMM_TASK does not use the task handle. */

    /* Create Humidity, Temperature and Pressure Task*/
    xTaskCreate( Task_BME280, "Humidity, Temperature and Pressure Task", 1000, NULL, 1, NULL ); /* This COMM_TASK does not use the task handle. */

    //Create timer for 4hz and register the timer_callback handler
    timer = xTimerCreate("1Hz Timer", pdMS_TO_TICKS(TIMER_PERIOD), pdTRUE, (void *)&timer_id, timer_callback);
    xTimerStart(timer, portMAX_DELAY);

    /* Start the scheduler so the tasks start executing. */
    vTaskStartScheduler();

/*    while(1)
    {
        if(packet_received)
        {
            packet_received = 0;
            UARTSend((uint8_t *)&receive_packet, RECEIVE_PACKET_SIZE);
        }

    }*/
}
