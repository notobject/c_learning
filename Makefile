CC = gcc
CFLAGS = -Wall -g 

SUBDIRS = demo_atomic demo_ipaddr

TARGET = c_learning

all: $(TARGET) sub-modules

sub-modules:
	@for subdir in $(SUBDIRS); do \
		make -j4 -C $$subdir all; \
		ret=$$?; \
		if [ ! $$ret -eq 0 ]; then \
			exit $$ret; \
		fi; \
	done;


$(TARGET): main.c
	$(CC) $(CFLAGS) -fstack-protector-all -g -o $(TARGET) main.c

clean:
	-@rm -f $(TARGET) *.o
	@for subdir in $(SUBDIRS); do \
		make -C $$subdir clean; \
		ret=$$?; \
		if [ ! $$ret -eq 0 ]; then \
			exit $$ret; \
		fi; \
	done;
