CC = gcc
CFLAGS = -c -o
CFLAGSs = -o

OBJS = LMServer.o Connect.o User.o LMSqlite.o
TARGET = LMServer

$(TARGET):$(OBJS)
	$(CC) $(OBJS) $(CFLAGSs) $(TARGET) -lsqlite3 -lpthread 

%.o:%.C
	$(CC) $(OBJS) $(CFLAGS) $^ $@

.PHONY:clean
clean:
	rm $(OBJS) $(TARGET)