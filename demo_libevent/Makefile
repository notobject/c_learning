CC = gcc
CFALGS = -g 

LD_INCLDE = -L/usr/lib/event2
LD_LIBS = -levent_core -lnet

TARGET = demo_libevent

all: $(TARGET)

$(TARGET): demo_libevent.c
	$(CC) $(CFALGS) $(LD_LIBS) $(LD_INCLDE) -o $(TARGET) demo_libevent.c


clean:
	@rm -rf *.o $(TARGET)