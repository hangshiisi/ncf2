#include <stdio.h> 

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <cmocka.h>
#include <libyang/libyang.h>


#include <session_server.h>

struct ly_ctx *ctx;

static int
setup_server(void **state)
{
    (void)state;

    ctx = ly_ctx_new(NULL);
    assert_non_null(ctx);

    nc_server_init(ctx);

    return 0;
}

static int
teardown_server(void **state)
{
    (void)state;

    nc_server_destroy();
    ly_ctx_destroy(ctx, NULL);

    return 0;
}

static void
test_dummy(void **state)
{
    (void)state;
}



int main() 
{ 
	printf("hello world\n"); 
	return 0; 
} 

