#include <stdbool.h>
#include <stdint.h>

#define BME280_SLAVE_ADDR		                0x76

#define BME280_CALIB00_ADDR                     0x88
#define BME280_CALIB26_ADDR                     0xE1

#define BME280_ID_ADDR                          0xD0
#define BME280_ID_VAL                           0x60

#define BME280_RESET_ADDR                       0xE0
#define BME280_RESET_VAL                        0xB6

#define BME280_CTRL_HUM_ADDR		            0xF2
#define BME280_CTRL_HUM_VAL_OVSP_SKIP           0x00        //Skip Humidity Reading
#define BME280_CTRL_HUM_VAL_OVSP_X1             0x01        //Humidity Oversampling Rate x1
#define BME280_CTRL_HUM_VAL_OVSP_X2		        0x02        //Humidity Oversampling Rate x2
#define BME280_CTRL_HUM_VAL_OVSP_X4             0x03        //Humidity Oversampling Rate x4
#define BME280_CTRL_HUM_VAL_OVSP_X8             0x04        //Humidity Oversampling Rate x8
#define BME280_CTRL_HUM_VAL_OVSP_X16            0x05        //Humidity Oversampling Rate x16

#define BME280_STATUS_ADDR                      0xF3        //Read only
#define BME280_STATUS_VAL_NOT_BUSY              0x00

#define BME280_CTRL_MEAS_ADDR                   0xF4
#define BME280_CTRL_MEAS_VAL_TEMP_OVSP_SKIP     0x00        //Skip Temperature Reading
#define BME280_CTRL_MEAS_VAL_TEMP_OVSP_X1       0x20        //Temperature Oversampling Rate x1
#define BME280_CTRL_MEAS_VAL_TEMP_OVSP_X2       0x40        //Temperature Oversampling Rate x2
#define BME280_CTRL_MEAS_VAL_TEMP_OVSP_X4       0x60        //Temperature Oversampling Rate x4
#define BME280_CTRL_MEAS_VAL_TEMP_OVSP_X8       0x80        //Temperature Oversampling Rate x8
#define BME280_CTRL_MEAS_VAL_TEMP_OVSP_X16      0xA0        //Temperature Oversampling Rate x16

#define BME280_CTRL_MEAS_VAL_PRESS_OVSP_SKIP    0x00        //Skip Pressure Reading
#define BME280_CTRL_MEAS_VAL_PRESS_OVSP_X1      0x04        //Pressure Oversampling Rate x1
#define BME280_CTRL_MEAS_VAL_PRESS_OVSP_X2      0x08        //Pressure Oversampling Rate x2
#define BME280_CTRL_MEAS_VAL_PRESS_OVSP_X4      0x0C        //Pressure Oversampling Rate x4
#define BME280_CTRL_MEAS_VAL_PRESS_OVSP_X8      0x10        //Pressure Oversampling Rate x8
#define BME280_CTRL_MEAS_VAL_PRESS_OVSP_X16     0x14        //Pressure Oversampling Rate x16

#define BME280_CTRL_MEAS_VAL_MODE_SLEEP         0x00        //Sleep Mode
#define BME280_CTRL_MEAS_VAL_MODE_FORCED        0x01        //Forced Mode
#define BME280_CTRL_MEAS_VAL_MODE_NORMAL        0x03        //Normal Mode

#define BME280_CONFIG_ADDR		                0xF5
#define BME280_CONFIG_VAL_T_STANDBY_500US		0x00
#define BME280_CONFIG_VAL_T_STANDBY_62500US     0x20
#define BME280_CONFIG_VAL_T_STANDBY_125MS       0x40
#define BME280_CONFIG_VAL_T_STANDBY_250MS       0x60
#define BME280_CONFIG_VAL_T_STANDBY_500MS       0x80
#define BME280_CONFIG_VAL_T_STANDBY_1000MS      0xA0
#define BME280_CONFIG_VAL_T_STANDBY_10MS        0xC0
#define BME280_CONFIG_VAL_T_STANDBY_20MS        0xE0

#define BME280_CONFIG_VAL_FILTER_OFF            0x00
#define BME280_CONFIG_VAL_FILTER_2              0x04
#define BME280_CONFIG_VAL_FILTER_4              0x08
#define BME280_CONFIG_VAL_FILTER_8              0x0C
#define BME280_CONFIG_VAL_FILTER_16             0x10

#define BME280_PRESS_MSB_ADDR                   0xF7
#define BME280_PRESS_LSB_ADDR                   0xF8
#define BME280_PRESS_XLSB_ADDR                  0xF9

#define BME280_TEMP_MSB_ADDR                    0xFA
#define BME280_TEMP_LSB_ADDR                    0xFB
#define BME280_TEMP_XLSB_ADDR	                0xFC

#define BME280_HUM_MSB_ADDR                    0xFD
#define BME280_HUM_LSB_ADDR                    0xFE

#define BME280_S32_t                            long signed int

typedef struct BME280_config
{
    uint8_t ctrl_hum;
    uint8_t ctrl_meas;
    uint8_t config;
    bool reset;
}BME280_config_t;

typedef struct BME280_data
{
    double temperature;
    double pressure;
    double humidity;
}BME280_data_t;

bool BME280_Init(void);
void BME280_Configure(BME280_config_t setup);
double BME280_compensate_T_double(BME280_S32_t adc_T);
double BME280_compensate_P_double(BME280_S32_t adc_P);
double bme280_compensate_H_double(BME280_S32_t adc_H);
BME280_data_t BME280_GetCompensatedData();
void Task_BME280(void *pvParameters);
