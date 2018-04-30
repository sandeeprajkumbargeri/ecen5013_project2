
#define TIMER_A_HALF_WIDTH_MAX_VAL         0xFFFFFFFF

void TIMER0_Init(void);
void TIMER1_Init(void);
void PortLIntHandler(void);
void PortHIntHandler(void);
void HCSR04_1_Init(void);
void HCSR04_2_Init(void);
void Task_HCSR04(void *pvParameters);
