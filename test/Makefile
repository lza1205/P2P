

#CC=gcc
#CC=mipsel-linux-gcc
#CC=/work/openwrt/trunk/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mipsel-openwrt-linux-gcc
#CC=i486-openwrt-linux-gcc

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


all:test_client

clean:
	rm -f *.o
	rm -f $(all)
	

test_client:client.o
	$(CC) $(LDFLAGS) $(INC) -o p2p_client client.o -lp2p-client
	
%.o:%.c
	$(CC) -c $(CPPFLAGS) $(INC) $(CCFLAGS) $< -o $@

	
