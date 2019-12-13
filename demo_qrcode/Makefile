CC = gcc
CFALGS = -g 

LD_INCLDE = -L/usr/local/lib
LD_LIBS = -lqrencode

all: demo_qrencode

demo_qrencode: demo_qrencode.c
	$(CC) $(CFALGS) $(LD_LIBS) $(LD_INCLDE) -o demo_qrencode demo_qrencode.c


clean:
	@rm -rf demo_qrencode *.o