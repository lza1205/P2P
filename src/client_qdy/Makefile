
CFLAGS = -g
CC = gcc

objects = client_demo.o qdy_aff.o qdy_interface.o

all:qdy_client

qdy_client: $(objects)
	gcc -o qdy_client $(objects)

%.o:%.c
	${CC} ${CFLAGS} -c -o $@ $^
	
clean:
	rm *.o qdy_client
	