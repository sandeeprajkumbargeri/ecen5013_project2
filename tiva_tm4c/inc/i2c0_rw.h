#include <stdio.h>
#include <stdint.h>

#define MODE_WRITE               false
#define MODE_READ                true

void I2C0_Init(void);
void I2C0_Write(uint8_t slave_addr, uint8_t reg_addr, uint8_t to_send);
void I2C0_Read(uint8_t slave_addr, uint8_t reg_addr, uint8_t *received, size_t length);
