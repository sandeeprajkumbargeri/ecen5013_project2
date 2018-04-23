IDIR=include/
SDIR=source/
CC=gcc
CFLAGS=-g -lm -lpthread -lrt
RFLAGS=-rf

execute: main.o sock_comm_task.o logger_task.o ui_task.o
	$(CC) -o execute main.o sock_comm_task.o logger_task.o ui_task.o $(CFLAGS)

sock_comm_task.o: $(SDIR)sock_comm_task.c $(IDIR)sock_comm_task.h main.h $(IDIR)logger_task.h
	$(CC) -c $(SDIR)sock_comm_task.c

logger_task.o: $(SDIR)logger_task.c $(IDIR)logger_task.h main.h
	$(CC) -c $(SDIR)logger_task.c

ui_task.o: ui_task.c ui_task.h $(IDIR)sock_comm_task.h
	$(CC) -c ui_task.c

main.o: main.c main.h $(IDIR)logger_task.h
	$(CC) -c main.c
.PHONY: clean

clean:
	rm $(RFLAGS) *.o $(SDIR)*.o execute
