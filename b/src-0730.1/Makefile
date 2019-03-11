

#CC=gcc
#CC=mipsel-linux-gcc
#CC=/work/openwrt/trunk/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mipsel-openwrt-linux-gcc
#CC=i486-openwrt-linux-gcc


CROSS_COMPILE = 
#CROSS_COMPILE = i486-openwrt-linux-

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm

STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump

export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP


CCFLAGS+=-g -O -Wall
LDFLAGS+=-g -O -Wall


ifeq ($(CC), mipsel-linux-gcc)
        INC = -L /work/openwrt/trunk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/openssl-1.0.2a/ipkg-install/usr/lib/ -L/work/openwrt/trunk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/json-c-0.12/ipkg-install/usr/lib/ -I /work/openwrt/trunk/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/usr/include/ -D_RT5350_
        LDLIB = -lpthread  -lssl -lcrypto -lm -ljson-c
        STAGING_DIR = /work/openwrt/trunk/
		CFLAGS += -I $(shell pwd)/include -I/usr/include/mysql -D_OPENWRT_
else ifeq ($(CC), i486-openwrt-linux-gcc)
        INC = -I/usr/include/mysql -L/usr/lib/mysql -L/work/p2p/lib/src -D_OPENWRT_ -I $(shell pwd)/include
        LDLIB = -lpthread -lssl -lcrypto -lm -ljson-c -ldl
		CFLAGS += -I $(shell pwd)/include -I/usr/include/mysql -D_OPENWRT_
else
        INC = -I/usr/include/mysql -L/work/p2p/lib/src -L/usr/lib/mysql -I $(shell pwd)/include
        LDLIB = -lpthread -lssl -lcrypto -lm -ljson
		CFLAGS += -I $(shell pwd)/include -I/usr/include/mysql
endif

SER_LDLIB+=-lpthread -lssl -lcrypto -lm -ljson -lmysqlclient
SER_INC = -I/usr/include/mysql -L/usr/lib/mysql

export CFLAGS LDFLAGS

TOPDIR := $(shell pwd)
export TOPDIR


TARGET := libp2p-server.so libp2p-server.a libp2p-client.so libp2p-client.a p2p_client p2p_server


obj-y += aes256/
obj-y += client/
obj-y += command/
obj-y += data/
obj-y += debug/
obj-y += hashtable/
obj-y += json/
obj-y += server/
obj-y += socket/
obj-y += list/

obj-client += aes256/built-in.o
obj-client += client/built-in.o
obj-client += command/built-in.o
obj-client += data/built-in.o
obj-client += debug/built-in.o
obj-client += json/built-in.o
obj-client += socket/built-in.o
obj-client += list/built-in.o

obj-server += aes256/built-in.o
obj-server += server/built-in.o
obj-server += command/built-in.o
obj-server += data/built-in.o
obj-server += debug/built-in.o
obj-server += json/built-in.o
obj-server += socket/built-in.o
obj-server += hashtable/built-in.o


all:libp2p-server.so libp2p-server.a libp2p-client.so libp2p-client.a p2p_client p2p_server

clean:
	rm -f $(shell find -name "*.o")
	rm -f $(TARGET)
	
libp2p-server.so:
	make -C ./ -f $(TOPDIR)/Makefile.build
	$(CC) $(LDFLAGS) $(INC) -shared -fPCI -o libp2p-server.so  $(obj-server) $(SER_LDLIB)
	$(AR) -rsv libp2p-server.a  $(obj-server)
	
libp2p-client.so:
	make -C ./ -f $(TOPDIR)/Makefile.build
	$(CC) $(LDFLAGS) $(INC) -shared -fPCI -o libp2p-client.so  $(obj-client) $(LDLIB)
	$(AR) -rsv libp2p-client.a   $(obj-client)

p2p_client:client.o
	$(CC) $(LDFLAGS) $(INC) -o p2p_client client.o -lp2p-client
	
p2p_server:server.o
	$(CC) $(LDFLAGS) $(INC) -o p2p_server server.o -lp2p-server
	
%.o:%.c
	$(CC) -c $(CPPFLAGS) $(INC) $(CCFLAGS) $< -o $@

	
