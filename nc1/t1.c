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


#include <session_client.h>
#include <session_server.h>
#include <messages_server.h> 
#include <messages_client.h> 
#include <session.h> 


// #include <session_p.h>
// #include <messages_p.h>

struct nc_session *server_session;
struct nc_session *client_session;
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



struct nc_server_reply *
my_get_rpc_clb(struct lyd_node *rpc, struct nc_session *session)
{
    assert_string_equal(rpc->schema->name, "get");
    assert_ptr_equal(session, server_session);

    return nc_server_reply_ok();
}

struct nc_server_reply *
my_getconfig_rpc_clb(struct lyd_node *rpc, struct nc_session *session)
{
    struct lyd_node *data;

    assert_string_equal(rpc->schema->name, "get-config");
    assert_ptr_equal(session, server_session);

    data = lyd_new_path(NULL, nc_session_get_ctx(session), 
    					"/ietf-netconf:get-config/data", NULL, 
    					LYD_PATH_OPT_OUTPUT);
    assert_non_null(data);

    return nc_server_reply_data(data, NC_PARAMTYPE_FREE);
}

static int
setup_sessions(void **state)
{
    
    

    return 0;
}

static int
teardown_sessions(void **state)
{
    

    return 0;
}



int main() 
{ 
	
	//ly_ctx_set_searchdir(&ctx)
    ctx = ly_ctx_new("/home/aurora/models");
    assert_non_null(ctx);

    ly_ctx_set_searchdir(ctx, "/home/aurora/models/ietf"); 
	lys_parse_path(ctx, "/home/aurora/models/ietf/ietf-netconf.yang", 
		 		LYS_IN_YANG); 

	lys_parse_path(ctx, "/home/aurora/models/ietf/ietf-netconf-monitoring.yang", 
		 		LYS_IN_YANG); 

    nc_server_init(ctx);









    nc_server_destroy();
    ly_ctx_destroy(ctx, NULL);


	printf("hello world\n"); 
	return 0; 
} 

