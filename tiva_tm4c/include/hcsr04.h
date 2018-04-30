
#define TIMER_A_HALF_WIDTH_MAX_VAL         0xFFFFFFFF

#define HCSR04_TASK_ACQ_EVENT    1<<0
#define HCSR04_TASK_RECEIVE_EVENT    1<<1

#define DOOR_LENGTH                        50

volatile int HCSR04_task_events;
int number_of_people;

void TIMER0_Init(void);
void TIMER1_Init(void);
void PortLIntHandler(void);
void PortHIntHandler(void);
void HCSR04_1_Init(void);
void HCSR04_2_Init(void);
void Task_HCSR04(void *pvParameters);
