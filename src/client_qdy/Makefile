
CFLAGS = -g
CC = gcc

objects = client_demo.o qdy_aff.o qdy_interface.o
objects_tcp = client_demo_tcp.o qdy_aff.o qdy_interface.o

all:qdy_client qdy_tcp_client

qdy_client: $(objects)
	gcc -o qdy_client $(objects)
	
qdy_tcp_client: $(objects_tcp)
	gcc -o qdy_tcp_client $(objects_tcp) -lpthread

%.o:%.c
	${CC} ${CFLAGS} -c -o $@ $^
	
clean:
	rm *.o qdy_client qdy_tcp_client
	