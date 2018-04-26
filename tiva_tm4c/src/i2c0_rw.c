#include <stdbool.h>

#include "inc/bme280.h"
#include "inc/i2c0_rw.h"

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

void I2C0_Init(void)
{
    // Enable the I2C0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Wait for the I2C0 module to be ready.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    //Configure pins for I2C0 Master Interface
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

    // Wait for the Peripheral to be ready for programming
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));

//     //Initialize and Configure the Master Module   - Use this when using Standard or Fast Speed.
///    //Doesn't work without added capacitance on the bus lines at 120 MHz
//    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), ui32SysClock), true);

    //Enable the I2C Master Module - for maximum speed (867 KHz (Fast Plus Speed) at 199.999 MHz)
    //Doesn't work without added capacitance on the bus lines at 120 MHz
    I2CMasterEnable(I2C0_BASE);

    //Enable glitch filtering for the fast mode plus I2C signals
    I2CMasterGlitchFilterConfigSet(I2C0_BASE, I2C_MASTER_GLITCH_FILTER_2);
}

//Function that implements writing a byte to a memory register inside the I2C slave device.
void I2C0_Write(uint8_t slave_addr, uint8_t reg_addr, uint8_t to_send)
{
    // Specify slave address with write mode
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, MODE_WRITE);

    // Place the character to be sent in the data register
    I2CMasterDataPut(I2C0_BASE, reg_addr);

    // Initiate send of character from Master to Slave
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

    // Delay until transmission completes
    while(I2CMasterBusy(I2C0_BASE));

    // Place the character to be sent in the data register
    I2CMasterDataPut(I2C0_BASE, to_send);

    // Initiate send of character from Master to Slave
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

    // Delay until transmission completes
    while(I2CMasterBusy(I2C0_BASE));
    while(I2CMasterBusBusy(I2C0_BASE));
}

//Function that implements reading 'length' number of sequential bytes from the slave device. Slave Device needs to support auto-increment sequential read.
void I2C0_Read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *received, size_t length)
{
    size_t flow_control = 0;

    // Specify slave address with write mode
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, MODE_WRITE);

    // Place the character to be sent in the data register
    I2CMasterDataPut(I2C0_BASE, reg_addr);

    // Initiate send of character from Master to Slave
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    // Delay until transmission completes
    while(I2CMasterBusy(I2C0_BASE));
    while(I2CMasterBusBusy(I2C0_BASE));

    // Specify slave address with write mode
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, MODE_READ);

    if(length == 1)
    {
        // Initiate send of character from Master to Slave
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

        // Delay until transmission completes
        while(I2CMasterBusy(I2C0_BASE));
        while(I2CMasterBusBusy(I2C0_BASE));

        *received = (uint8_t) I2CMasterDataGet(I2C0_BASE);
    }

    else
    {
        flow_control = 0;

        do
        {
            if(flow_control == 0)
            {
                // Initiate send of character from Master to Slave
                I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
            }

            else if(flow_control < length - 1)
            {
                // Initiate send of character from Master to Slave
                I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
            }

            else
            {
                // Initiate send of character from Master to Slave
                I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
                while(I2CMasterBusy(I2C0_BASE));
                while(I2CMasterBusBusy(I2C0_BASE));
            }

            // Delay until transmission completes
            while(I2CMasterBusy(I2C0_BASE));

             //Copy down the received data into the buffer
            *received = (uint8_t) I2CMasterDataGet(I2C0_BASE);
            received++;
            flow_control++;

            while(I2CMasterBusy(I2C0_BASE));

        }while(flow_control < length);
    }
}
