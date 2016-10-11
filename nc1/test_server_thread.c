/**
 * \file test_server_thread.c
 * \author Michal Vasko <mvasko@cesnet.cz>
 * \brief libnetconf2 tests - thread-safety of all server functions
 *
 * Copyright (c) 2015 CESNET, z.s.p.o.
 *
 * This source code is licensed under BSD 3-Clause License (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://opensource.org/licenses/BSD-3-Clause
 */

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <libyang/libyang.h>

/* needed by libnetconf2 lib */ 
#ifndef NC_ENABLED_SSH 
#define NC_ENABLED_SSH
#endif 
#include <libnetconf2/session_client.h>
#include <libnetconf2/session_server.h>
#include <libnetconf2/log.h>

/* millisec */
#define NC_ACCEPT_TIMEOUT 5000
/* millisec */
#define NC_PS_POLL_TIMEOUT 5000
/* sec */
#define CLIENT_SSH_AUTH_TIMEOUT 10

#define nc_assert(cond) if (!(cond)) { fprintf(stderr, "assert failed (%s:%d)\n", __FILE__, __LINE__); exit(1); }

pthread_barrier_t barrier;


static void *
server_thread(void *arg)
{
    (void)arg;
    NC_MSG_TYPE msgtype;
    int ret;
    struct nc_pollsession *ps;
    struct nc_session *session;

    ps = nc_ps_new();
    nc_assert(ps);

    pthread_barrier_wait(&barrier);


    msgtype = nc_accept(NC_ACCEPT_TIMEOUT, &session);
    nc_assert(msgtype == NC_MSG_HELLO);

    nc_ps_add_session(ps, session);
    ret = nc_ps_poll(ps, NC_PS_POLL_TIMEOUT, NULL);
    nc_assert(ret & NC_PSPOLL_RPC);
    nc_ps_clear(ps, 0, NULL);

    nc_ps_free(ps);

    nc_thread_destroy();
    return NULL;
}


static void *
ssh_add_endpt_thread(void *arg)
{
    (void)arg;
    int ret;

    pthread_barrier_wait(&barrier);

    ret = nc_server_ssh_add_endpt_listen("tertiary", "0.0.0.0", 6003);
    nc_assert(!ret);

    return NULL;
}

static void *
ssh_endpt_set_port_thread(void *arg)
{
    (void)arg;
    int ret;

    pthread_barrier_wait(&barrier);

    ret = nc_server_ssh_endpt_set_port("quaternary", 6005);
    nc_assert(!ret);

    return NULL;
}

static void *
ssh_del_endpt_thread(void *arg)
{
    (void)arg;
    int ret;

    pthread_barrier_wait(&barrier);

    ret = nc_server_ssh_del_endpt("secondary");
    nc_assert(!ret);

    return NULL;
}

static void *
ssh_endpt_set_hostkey_thread(void *arg)
{
    (void)arg;
    int ret;

    pthread_barrier_wait(&barrier);

    ret = nc_server_ssh_endpt_set_hostkey("main", "/home/aurora/aurora_pub_key");
    nc_assert(!ret);

    return NULL;
}

static void *
ssh_endpt_set_banner_thread(void *arg)
{
    (void)arg;
    int ret;

    pthread_barrier_wait(&barrier);

    ret = nc_server_ssh_endpt_set_banner("main", "Howdy, partner!");
    nc_assert(!ret);

    return NULL;
}

static void *
ssh_endpt_set_auth_methods_thread(void *arg)
{
    (void)arg;
    int ret;

    pthread_barrier_wait(&barrier);

    ret = nc_server_ssh_endpt_set_auth_methods("main", NC_SSH_AUTH_PUBLICKEY | NC_SSH_AUTH_PASSWORD | NC_SSH_AUTH_INTERACTIVE);
    nc_assert(!ret);

    return NULL;
}

static void *
ssh_endpt_set_auth_attempts_thread(void *arg)
{
    (void)arg;
    int ret;

    pthread_barrier_wait(&barrier);

    ret = nc_server_ssh_endpt_set_auth_attempts("main", 2);
    nc_assert(!ret);

    return NULL;
}

static void *
ssh_endpt_set_auth_timeout_thread(void *arg)
{
    (void)arg;
    int ret;

    pthread_barrier_wait(&barrier);

    ret = nc_server_ssh_endpt_set_auth_timeout("main", 5);
    nc_assert(!ret);

    return NULL;
}

static void *
ssh_endpt_add_authkey_thread(void *arg)
{
    (void)arg;
    int ret;

    pthread_barrier_wait(&barrier);

    ret = nc_server_ssh_endpt_add_authkey("main", "/home/aurora/aurora_pub_key", "aurora");
    nc_assert(!ret);

    return NULL;
}

static void *
ssh_endpt_del_authkey_thread(void *arg)
{
    (void)arg;
    int ret;

    pthread_barrier_wait(&barrier);

    ret = nc_server_ssh_endpt_del_authkey("main", "/home/aurora/aurora_pub_key", "aurora");
    nc_assert(!ret);

    return NULL;
}

static int
ssh_hostkey_check_clb(const char *hostname, ssh_session session)
{
    (void)hostname;
    (void)session;

    return 0;
}

static void *
ssh_client_thread(void *arg)
{
    int ret, read_pipe = *(int *)arg;
    char buf[9];
    struct nc_session *session;

    ret = read(read_pipe, buf, 9);
    nc_assert(ret == 9);
    nc_assert(!strncmp(buf, "ssh_ready", 9));

    /* skip the knownhost check */
    nc_client_ssh_set_auth_hostkey_check_clb(ssh_hostkey_check_clb);

    ret = nc_client_ssh_set_username("test");
    nc_assert(!ret);

    ret = nc_client_ssh_add_keypair("/home/aurora/aurora_pub_key", "aurora");
    nc_assert(!ret);

    nc_client_ssh_set_auth_pref(NC_SSH_AUTH_PUBLICKEY, 1);
    nc_client_ssh_set_auth_pref(NC_SSH_AUTH_PASSWORD, -1);
    nc_client_ssh_set_auth_pref(NC_SSH_AUTH_INTERACTIVE, -1);

    session = nc_connect_ssh("127.0.0.1", 6001, NULL);
    nc_assert(session);

    nc_session_free(session, NULL);

    nc_thread_destroy();
    return NULL;
}


static void *(*thread_funcs[])(void *) = {
    server_thread,
    ssh_add_endpt_thread,
    ssh_endpt_set_port_thread,
    ssh_del_endpt_thread,
    ssh_endpt_set_hostkey_thread,
    ssh_endpt_set_banner_thread,
    ssh_endpt_set_auth_methods_thread,
    ssh_endpt_set_auth_attempts_thread,
    ssh_endpt_set_auth_timeout_thread,
    ssh_endpt_add_authkey_thread,
    ssh_endpt_del_authkey_thread,
};

const int thread_count = sizeof thread_funcs / sizeof *thread_funcs;

const int client_count = 1;
pid_t pids[1];
int pipes[2];

static void
client_fork(void)
{
    int ret, clients = 0;

    if (-1 == pipe(pipes + clients * 2)) { 
        perror("pipe"); 
        exit(EXIT_FAILURE); 
    } 

    if (!(pids[clients] = fork())) {
        nc_client_init();

        ret = nc_client_set_schema_searchpath("../schemas");
        nc_assert(!ret);

        /* close write */
        close(pipes[clients * 2 + 1]);
        ssh_client_thread(&pipes[clients * 2]);
        close(pipes[clients * 2]);
        nc_client_destroy();
        exit(0);
    }
    /* close read */
    close(pipes[clients * 2]);

    ++clients;

}

int
main(void)
{
    struct ly_ctx *ctx;
    int ret, i, clients = 0;
    pthread_t tids[thread_count];

    nc_verbosity(NC_VERB_VERBOSE);

    client_fork();

    ctx = ly_ctx_new("../schemas");
    nc_assert(ctx);
    ly_ctx_load_module(ctx, "ietf-netconf", NULL);

    lys_parse_path(ctx, "/home/aurora/models/ietf/ietf-netconf.yang", 
                    LYS_IN_YANG); 

    lys_parse_path(ctx, "/home/aurora/models/ietf/ietf-netconf-monitoring.yang", 
                    LYS_IN_YANG); 


    nc_server_init(ctx);

    printf("Hello world 1 \n"); 

    pthread_barrier_init(&barrier, NULL, thread_count);


    /* do first, so that client can connect on SSH */
    ret = nc_server_ssh_add_endpt_listen("main", "0.0.0.0", 6001);
    printf("Hello world 1.1 \n"); 
    nc_assert(!ret);
    printf("Hello world 1.2 \n"); 
    ret = nc_server_ssh_endpt_add_authkey("main", "/home/aurora/.ssh/id_rsa.pub", "aurora");
    printf("Hello world 1.3 \n"); 
    nc_assert(!ret);
    printf("Hello world 1.4 \n"); 
    ret = nc_server_ssh_endpt_set_hostkey("main", "/home/aurora/.ssh/id_rsa");
    printf("Hello world 1.5 \n"); 
    nc_assert(!ret);
    printf("Hello world 1.6 \n"); 

    /* client ready */
    ret = write(pipes[clients * 2 + 1], "ssh_ready", 9);
    nc_assert(ret == 9);
    printf("Hello world 1.7 \n"); 
    ++clients;

    printf("Hello world 2 \n"); 



    /* for ssh_endpt_del_authkey */
    ret = nc_server_ssh_endpt_add_authkey("main", "/data/key_ecdsa.pub", "test2");
    nc_assert(!ret);

    /* for ssh_del_endpt */
    ret = nc_server_ssh_add_endpt_listen("secondary", "0.0.0.0", 6002);
    nc_assert(!ret);

    /* for ssh_endpt_set_port */
    ret = nc_server_ssh_add_endpt_listen("quaternary", "0.0.0.0", 6004);
    nc_assert(!ret);


    printf("Hello world 3 \n"); 



    /* threads'n'stuff */
    ret = 0;
    for (i = 0; i < thread_count; ++i) {
        ret += pthread_create(&tids[i], NULL, thread_funcs[i], NULL);
    }
    nc_assert(!ret);

    printf("Hello world 4 \n"); 


    /* cleanup */
    for (i = 0; i < thread_count; ++i) {
        pthread_join(tids[i], NULL);
        printf("Hello world 5 \n"); 
    }
    for (i = 0; i < client_count; ++i) {
        waitpid(pids[i], NULL, 0);
        printf("Hello world 6 \n"); 
        close(pipes[i * 2 + 1]);
    }


    printf("Hello world 7 \n"); 

    //pthread_barrier_destroy(&barrier);

    nc_server_destroy();

    printf("Hello world 8 \n"); 

    ly_ctx_destroy(ctx, NULL);

    return 0;
}
