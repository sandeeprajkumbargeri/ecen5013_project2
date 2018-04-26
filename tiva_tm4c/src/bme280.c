#include <stdbool.h>

#include "inc/i2c0_rw.h"
#include "inc/bme280.h"

unsigned    short   dig_T1;
signed      short   dig_T2;
signed      short   dig_T3;

unsigned    short   dig_P1;
signed      short   dig_P2;
signed      short   dig_P3;
signed      short   dig_P4;
signed      short   dig_P5;
signed      short   dig_P6;
signed      short   dig_P7;
signed      short   dig_P8;
signed      short   dig_P9;

unsigned    char    dig_H1;
signed      short   dig_H2;
unsigned    char    dig_H3;
signed      short   dig_H4;
signed      short   dig_H5;
signed      char    dig_H6;

long signed int t_fine = 0x00;

bool BME280_Init(void)
{
    uint8_t device_id = 0;
    uint8_t calib_dataset_1_[26] = {0};
    uint8_t calib_dataset_2_[7] = {0};

    I2C0_Read(BME280_SLAVE_ADDR, BME280_ID_ADDR, &device_id, 1);

    if(device_id != BME280_ID_VAL)
        return false;

    I2C0_Read(BME280_SLAVE_ADDR, BME280_CALIB00_ADDR, calib_dataset_1_, 26);
    I2C0_Read(BME280_SLAVE_ADDR, BME280_CALIB26_ADDR, calib_dataset_2_, 7);

    dig_T1 = (unsigned    short) (calib_dataset_1_[1] << 8) | calib_dataset_1_[0];
    dig_T2 = (signed      short) (calib_dataset_1_[3] << 8) | calib_dataset_1_[2];
    dig_T3 = (signed      short) (calib_dataset_1_[5] << 8) | calib_dataset_1_[4];

    dig_P1 = (unsigned    short) (calib_dataset_1_[7] << 8) | calib_dataset_1_[6];
    dig_P2 = (signed      short) (calib_dataset_1_[9] << 8) | calib_dataset_1_[8];
    dig_P3 = (signed      short) (calib_dataset_1_[11] << 8) | calib_dataset_1_[10];
    dig_P4 = (signed      short) (calib_dataset_1_[13] << 8) | calib_dataset_1_[12];
    dig_P5 = (signed      short) (calib_dataset_1_[15] << 8) | calib_dataset_1_[14];
    dig_P6 = (signed      short) (calib_dataset_1_[17] << 8) | calib_dataset_1_[16];
    dig_P7 = (signed      short) (calib_dataset_1_[19] << 8) | calib_dataset_1_[18];
    dig_P8 = (signed      short) (calib_dataset_1_[21] << 8) | calib_dataset_1_[20];
    dig_P9 = (signed      short) (calib_dataset_1_[23] << 8) | calib_dataset_1_[22];

    dig_H1 = (unsigned    char)  (calib_dataset_1_[25]);
    dig_H2 = (signed      short) (calib_dataset_2_[1] << 8) | calib_dataset_2_[0];
    dig_H3 = (unsigned    char)  (calib_dataset_2_[2]);
    dig_H4 = (signed      short) (calib_dataset_2_[3] << 4) | (calib_dataset_2_[4] & 0x0F);
    dig_H5 = (signed      short) (calib_dataset_2_[5] << 4) | ((calib_dataset_2_[4] & 0xF0) >> 4);
    dig_H6 = (unsigned    char)  (calib_dataset_2_[6]);

    return true;
}

void BME280_Configure(BME280_config_t setup)
{
    uint8_t bme280_mode = 0;

    if(setup.reset == true)
        I2C0_Write(BME280_SLAVE_ADDR, BME280_RESET_ADDR, BME280_RESET_VAL);

    I2C0_Read(BME280_SLAVE_ADDR, BME280_CTRL_MEAS_ADDR, &bme280_mode, 1);

    if(bme280_mode & 0x03 != BME280_CTRL_MEAS_VAL_MODE_SLEEP)
        I2C0_Write(BME280_SLAVE_ADDR, BME280_CTRL_MEAS_ADDR, (bme280_mode & 0xFC));

    I2C0_Write(BME280_SLAVE_ADDR, BME280_CTRL_HUM_ADDR, setup.ctrl_hum);

    I2C0_Write(BME280_SLAVE_ADDR, BME280_CONFIG_ADDR, setup.config);

    I2C0_Write(BME280_SLAVE_ADDR, BME280_CTRL_MEAS_ADDR, setup.ctrl_meas);
}

/* The following three compensation functions are taken from BME280 datasheet*/

// Returns temperature in DegC, double precision. Output value of “51.23” equals 51.23 DegC.
// t_fine carries fine temperature as global value
double BME280_compensate_T_double(BME280_S32_t adc_T)
{
    double var1, var2, T;
    var1 = (((double)adc_T)/16384.0 - ((double)dig_T1)/1024.0) * ((double)dig_T2);
    var2 = ((((double)adc_T)/131072.0 - ((double)dig_T1)/8192.0) * (((double)adc_T)/131072.0 - ((double) dig_T1)/8192.0)) * ((double)dig_T3);
    t_fine = (BME280_S32_t)(var1 + var2);
    T = (var1 + var2) / 5120.0;
    return T;
}

// Returns pressure in Pa as double. Output value of “96386.2” equals 96386.2 Pa = 963.862 hPa
double BME280_compensate_P_double(BME280_S32_t adc_P)
{
    double var1, var2, p;
    var1 = ((double)t_fine/2.0) - 64000.0;
    var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)dig_P5) * 2.0;
    var2 = (var2/4.0)+(((double)dig_P4) * 65536.0);
    var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0)*((double)dig_P1);

    if (var1 == 0.0)
    {
        return 0; // avoid exception caused by division by zero
    }

    p = 1048576.0 - (double)adc_P;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double)dig_P7)) / 16.0;
    return p;
}
// Returns humidity in %rH as as double. Output value of “46.332” represents 46.332 %rH
double bme280_compensate_H_double(BME280_S32_t adc_H)
{
    double var_H;
    var_H = (((double)t_fine) - 76800.0);
    var_H = (adc_H - (((double)dig_H4) * 64.0 + ((double)dig_H5) / 16384.0 * var_H)) * (((double)dig_H2) / 65536.0 * (1.0 + ((double)dig_H6) / 67108864.0 * var_H * (1.0 + ((double)dig_H3) / 67108864.0 * var_H)));
    var_H = var_H * (1.0 - ((double)dig_H1) * var_H / 524288.0);

    if (var_H > 100.0)
        var_H = 100.0;

    else if (var_H < 0.0)
        var_H = 0.0;

    return var_H;
}

BME280_data_t BME280_GetCompensatedData()
{
    uint8_t bme280_status = 0;
    uint8_t read_bytes[8] = {0};
    signed long int t_raw = 0;
    signed long int p_raw = 0;
    signed long int h_raw = 0;
    BME280_data_t compensated_data;

    do
    {
        I2C0_Read(BME280_SLAVE_ADDR, BME280_STATUS_ADDR, &bme280_status, 1);
    }while(bme280_status != BME280_STATUS_VAL_NOT_BUSY);

    I2C0_Read(BME280_SLAVE_ADDR, BME280_PRESS_MSB_ADDR, read_bytes, 8);

    p_raw = ((signed long int) read_bytes[0] << 12) | ((signed long int) read_bytes[1] << 4) | (read_bytes[2] >> 4);
    t_raw = ((signed long int) read_bytes[3] << 12) | ((signed long int) read_bytes[4] << 4) | (read_bytes[5] >> 4);
    h_raw = ((signed long int) read_bytes[6] << 8) | ((signed long int) read_bytes[7]);

    compensated_data.temperature = BME280_compensate_T_double(t_raw);
    compensated_data.pressure = BME280_compensate_P_double(p_raw);
    compensated_data.humidity = bme280_compensate_H_double(h_raw);

    return compensated_data;
}

