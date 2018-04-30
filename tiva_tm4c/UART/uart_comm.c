/*
 * uart_comm.c
 *
 *  Created on: Apr 28, 2018
 *      Author: Prithvi Teja
 */

#include "uart_comm.h"
#include "../include/comm_task.h"
#include <stdint.h>

#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

void UARTStdioIntHandler(void)
{
    static int bytes_received = 0;
    uint8_t *packet_ptr;

    packet_ptr = (uint8_t *)&receive_packet;

    UARTIntClear(UART0_BASE, UART_INT_RX);

    while(UARTCharsAvail(UART0_BASE))
    {
        packet_ptr[bytes_received] = UARTCharGet(UART0_BASE);
        if(++bytes_received == RECEIVE_PACKET_SIZE)
        {
            xQueueSend(comm_receive_queue, (void *)&receive_packet, portMAX_DELAY);
            if(!(comm_task_events & COMM_TASK_RECEIVE_EVENT))
            {
                xSemaphoreGive(comm_task_sem);
                comm_task_events |= COMM_TASK_RECEIVE_EVENT;
            }
            bytes_received = 0;
        }
    }
}


void UART_Configure(uint32_t ui32SrcClock)
{
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

//    UARTIntRegister(UART0_BASE, UARTStdioIntHandler);
//    UARTIntEnable(UART0_BASE, UART_INT_RX);

    UARTStdioConfig(UART_PORT, BAUD_RATE, ui32SrcClock);
    UARTIntRegister(UART0_BASE, UARTStdioIntHandler);
    UARTIntEnable(UART0_BASE, UART_INT_RX);
}

void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    // Loop while there are more characters to send.
    while(ui32Count--)
    {
        // Write the next character to the UART.
        UARTCharPut(UART0_BASE, *pui8Buffer++);
        if(*(pui8Buffer - 1) == 0)
            break;
    }
}
