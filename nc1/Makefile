

# usr/local/include/libnetconf 
CFLAGS := -std=c99 -O2 -I/usr/local/include/libnetconf2 


all: ncf_server  

debug: CFLAGS := -std=c99 -g -O0 $(CFLAGS)
debug: ncf_server


clean:
	rm ncf_server ncf_client 


ncf_server: t1.c  
	gcc $? $(CFLAGS) -lcmocka -lnetconf2 -lyang -o $@




