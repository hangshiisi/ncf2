

# usr/local/include/libnetconf 
#CFLAGS := -std=c99 -O2 -I/usr/local/include/libnetconf2 -I/usr
CFLAGS =  -O2 


all: ncf_server  tydict tyang tserver

debug: CFLAGS := -std=c99 -g -O0 $(CFLAGS)
debug: ncf_server


clean:
	rm -rf ncf_server ncf_client 


ncf_server: t1.c  
	gcc $? $(CFLAGS) -lcmocka -lnetconf2 -lyang -o $@


tydict: tydict.c  
	gcc $? $(CFLAGS) -lcmocka -lnetconf2 -lyang -o $@



tyang: tyang.c  
	gcc $? $(CFLAGS) -lcmocka -lnetconf2 -lyang -o $@


tserver: test_server_thread.c  
	gcc $? $(CFLAGS) -lcmocka -lnetconf2 -lyang -lpthread -o $@


tsshd: tsshd.c  
	gcc $? $(CFLAGS) -lcmocka -lnetconf2 -lyang -lpthread -lssh -o $@

