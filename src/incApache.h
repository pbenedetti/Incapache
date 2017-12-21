#ifndef incApache_h
#  define incApache_h

/* 
 * incApache.h: definizioni per il web server
 *
 * versione 1.0 del 03/12/2012
 *
 * Programma sviluppato a supporto del laboratorio di
 * Sistemi di Elaborazione e Trasmissione del corso di laurea
 * in Informatica classe L-31 presso l'Università degli Studi di
 * Genova, anno accademico 2012/2013.
 *
 * Copyright (C) 2012 by Giovanni Chiola <chiolag@acm.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/**/
#  define HTTP_1_1


#  define _XOPEN_SOURCE /* glibc2 needs this */
#  define _XOPEN_SOURCE_EXTENDED /* glibc2 needs this */

#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#  include <unistd.h>
#  include <time.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netdb.h>
#  include <netinet/in.h>
#  include <netinet/tcp.h>
#  include <arpa/inet.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#  include <dirent.h>
#  include <libgen.h>
#  include <pthread.h>

#  define MAXCONNECTIONS 8

#  ifdef HTTP_1_1
#      define MAXTHREADS 4*MAXCONNECTIONS /*** this should be > 2*MAXCONNECTION ***/

       typedef struct answ_par_str {
           int ap_answer_no;
           int ap_http1_0;
           char * ap_filename;
           struct stat * ap_stat;
       } Answer_params;
#  endif

#  define MAXINTSTR 20
#  define MAXTIMESTR 40
#  define MAXLINE 2000
#  define MAXHEADER 10000
#  define BACKLOG 10

#  define PIPE_READ_END 0
#  define PIPE_WRITE_END 1

   extern FILE * mreq_f, * mansw_f;
   extern int listenSocketFD;

   extern pthread_t thread_id[];
   extern int connection_no[];
   extern pthread_mutex_t my_accept_mutex;
   extern pthread_mutex_t my_mime_mutex;

#  ifdef HTTP_1_1
    extern int client_socket[];
    extern pthread_t * to_join[];
    extern pthread_mutex_t my_threads_mutex;
    extern int no_threads[];
    extern int no_free_threads;
    extern Answer_params thread_params[];

    extern int get_new_thread(int conn_no);
    extern void join_all_threads(int conn_no);
    extern void join_prev_thread(int thrd_no);
#  endif

   extern void fail_errno(char *);
   extern void fail(char *);
   extern void * check_malloc(void*);

   extern void * client_connection_thread( void * vp );
   extern char * get_mime_type( char * filename );

#    ifdef HTTP_1_1
       extern void send_resp_thread( int out_socket, int http_response,
                                     int http10, int my_conn_no, int my_thrd,
                                     char * filename, struct stat * stat_p);
#    endif

   extern void send_response(int out_socket, int http_response,
#    ifdef HTTP_1_1
                             int http10, int my_conn_no,
#    endif
                             char * filename, struct stat * stat_p);
   extern void manage_http_requests( int my_socket
#    ifdef HTTP_1_1
                                   , int my_conn_no
#    endif
                                   );


#endif
