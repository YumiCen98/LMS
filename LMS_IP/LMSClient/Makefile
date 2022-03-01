CC = gcc
CFLAGS = -c -o
CFLAGSs = -o

OBJS = LMSClient.o Connect.o User.o Operation.o LoginUi.o
TARGET = LMSClient

$(TARGET):$(OBJS)
	$(CC) $(OBJS) $(CFLAGSs) $(TARGET)

%.o:%.C
	$(CC) $(OBJS) $(CFLAGS) $^ $@

.PHONY:clean
clean:
	rm $(OBJS) $(TARGET)