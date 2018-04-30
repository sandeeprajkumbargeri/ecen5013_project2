#include "main.h"

#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include <math.h>
#include <sys/ioctl.h>
#include <sys/signal.h>

#include "include/logger_task.h"
#include "include/sock_comm_task.h"
#include "include/ui_task.h"

pthread_t logger_task;
pthread_t sock_comm_task;
pthread_t ui_task;

bool task_alive[TOTAL_TASKS] = {true};
bool task_heartbeat[TOTAL_TASKS] = {false};
bool send_heartbeat[TOTAL_TASKS] = {true};

char task_name[TOTAL_TASKS][30];
char filename[64];
bool close_app = false;

struct sigevent heartbeat_sevp;

sem_t sem_logger, sem_sock_comm, sem_ui;
mqd_t mq_logger, mq_heartbeat;

int sock_ui, sock_comm, sock_comm_ip;

void signal_handler(int signal)
{
  close_app = true;
}

int main (int argc, char *argv[])
{
	timer_t timer_id;
  struct sigevent sevp;
	struct itimerspec tspec;
	pthread_args_t thread_args;
	int retval;
	mq_payload_heartbeat_t heartbeat;
	char log_message[128] = {0};

	signal(SIGINT, signal_handler);

	if (argc != 2)
  {
    printf ("USAGE: %s <log filename>\n", argv[0]);
    exit(1);
  }

	bzero(filename, sizeof(filename));
	strcpy(filename, argv[1]);

  //Open the log file in the write mode
  log_file = fopen(argv[1], "w");
	if(log_file == NULL)
  {
    printf ("\n## ERROR ## Cannot create the file \"%s\". Exiting.\n", argv[1]);
    exit(1);
  }

	fclose(log_file);

	for (int i = 0; i < TOTAL_TASKS; i++)
	{
		task_alive[i] = true;
		task_heartbeat[i] = false;
		send_heartbeat[i] = true;
	}

	setup_mq();
	bzero(log_message, sizeof(log_message));
	strcpy(log_message, "## MAIN ## Successfully setup message queues.");
	LOG(mq_logger, log_message);

	sem_init(&sem_logger, 0, 0);
	sem_init(&sem_sock_comm, 0, 0);

	bzero(log_message, sizeof(log_message));
	strcpy(log_message, "## MAIN ## Successfully setup semaphores.");
	LOG(mq_logger, log_message);

	//Set the timer configuration
	sevp.sigev_notify = SIGEV_THREAD;
	sevp.sigev_value.sival_ptr = &timer_id;
	sevp.sigev_notify_function = timer_expiry_handler;
	sevp.sigev_notify_attributes = NULL;

	bzero(task_name, sizeof(task_name));
	strcpy(task_name[0], LOGGER_TASK_NAME);
	strcpy(task_name[1], SOCK_COMM_TASK_NAME);
  strcpy(task_name[2], UI_TASK_NAME);

	bzero(log_message, sizeof(log_message));
	sprintf(log_message, "## MAIN ## Successfully setup timer with %u sec %u msec expiry time.", TIMER_EXPIRY_SEC, TIMER_EXPIRY_MSEC);
	LOG(mq_logger, log_message);

	pthread_create(&logger_task, NULL, logger_task_thread, (void *) &thread_args);
	pthread_create(&sock_comm_task, NULL, sock_comm_task_thread, (void *) &thread_args);
  pthread_create(&ui_task, NULL, ui_task_thread, (void *) &thread_args);

	bzero(log_message, sizeof(log_message));
	strcpy(log_message, "## MAIN ## Successfully created all the threads.");
	LOG(mq_logger, log_message);

  //Set the timer value to 500ms
	tspec.it_value.tv_sec = TIMER_EXPIRY_SEC;
	tspec.it_value.tv_nsec = TIMER_EXPIRY_MSEC *1000000;
	tspec.it_interval.tv_sec = TIMER_EXPIRY_SEC;
	tspec.it_interval.tv_nsec = TIMER_EXPIRY_MSEC *1000000;

	//Timer creation and setting alarm
	timer_create(CLOCK_ID, &sevp, &timer_id);
	timer_settime(timer_id, 0, &tspec, 0);

	while(!close_app)
	{
		retval = mq_receive(mq_heartbeat, (char *) &heartbeat, sizeof(heartbeat), NULL);

		if(retval > 0)
		{
			task_heartbeat[heartbeat.sender_id] = heartbeat.heartbeat_status;

			bzero(log_message, sizeof(log_message));
			sprintf(log_message, "## MAIN ## Received heartbeat from %s. Status: %s.", task_name[(uint8_t) heartbeat.sender_id], heartbeat.heartbeat_status? "good":"bad");
			LOG(mq_logger, log_message);
		}
	}

	printf("\n## SIGINT ## Clean up threads, semaphores, sockets and message queues.\n");

	pthread_join(logger_task, NULL);
	pthread_join(sock_comm_task, NULL);
  pthread_join(ui_task, NULL);

	mq_unlink(MQ_LOGGER_PATH);
	mq_unlink(MQ_HEARTBEAT_PATH);

  close(sock_comm);
  close(sock_ui);

	sem_destroy(&sem_logger);
	sem_destroy(&sem_sock_comm);
  sem_destroy(&sem_ui);

	printf("\n## SIGINT ## Process exited successfully.\n");
	exit(EXIT_SUCCESS);
}


void timer_expiry_handler(union sigval arg)
{
	static uint8_t count = 0;
	char log_message[128] = {0};

	if(count % 3 == 0)
	{
		count++;
		sem_post(&sem_logger);
	}

  else if(count % 3 == 1)
  {
    count++;
    sem_post(&sem_sock_comm);
  }

	else
	{
		count++;
		sem_post(&sem_ui);
	}

	if(count == 3)
	{
		count = 0;

		for(int i = 0; i < TOTAL_TASKS; i++)
		{
			if(task_alive[i])
			{
				if(task_heartbeat[i])
				{
					bzero(log_message, sizeof(log_message));
					sprintf(log_message, "## MAIN ## %s is still alive.", task_name[i]);
					LOG(mq_logger, log_message);

					send_heartbeat[i] = true;
					task_alive[i] = true;
				}

				else
				{
					task_alive[i] = false;

					bzero(log_message, sizeof(log_message));
					sprintf(log_message, "## MAIN ## %s is dead. Failed to provide its heartbeat.", task_name[i]);
					LOG(mq_logger, log_message);
				}
			}
		}
	}
}

void setup_mq(void)
{
	struct mq_attr attr;

	mq_unlink(MQ_LOGGER_PATH);
	mq_unlink(MQ_HEARTBEAT_PATH);

	bzero(&attr, sizeof(attr));
	attr.mq_flags = O_RDWR;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(mq_payload_heartbeat_t);

	mq_heartbeat = mq_open(MQ_HEARTBEAT_PATH, O_CREAT | O_RDWR | S_IRUSR, S_IWUSR | S_IROTH | S_IWOTH, &attr);

	if(mq_heartbeat < 0)
		perror("Heartbeat Message Queue");

	attr.mq_msgsize = 128 * sizeof(char);

	mq_logger = mq_open(MQ_LOGGER_PATH, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);

	if(mq_logger < 0)
		perror("Logger Queue");
}
