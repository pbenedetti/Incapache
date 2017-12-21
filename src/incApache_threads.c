/* 
 * incApache_threads.c: implementazione dei thread per il web server del corso di SET
 *
 * versione 1.0 del 03/12/2012
 *
 * Programma sviluppato a supporto del laboratorio di
 * Sistemi di Elaborazione e Trasmissione del corso di laurea
 * Genova, anno accademico 2012/2013.
 *
 * Copyright (C) 2012 by Giovanni Chiola <chiolag@acm.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
#define DEBUG
*/


#include "incApache.h"


pthread_mutex_t my_accept_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t my_mime_mutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef HTTP_1_1
  int client_socket[MAXCONNECTIONS];
  pthread_t thread_id[MAXTHREADS];
  int connection_no[MAXTHREADS];
  pthread_t * to_join[MAXTHREADS];
  pthread_mutex_t my_threads_mutex = PTHREAD_MUTEX_INITIALIZER;
  int no_threads[MAXCONNECTIONS];
  int no_free_threads = MAXTHREADS - 2*MAXCONNECTIONS;
  Answer_params thread_params[MAXTHREADS-MAXCONNECTIONS];


int get_new_thread(int conn_no) {
    int got_it = -1;
    if ( no_threads[conn_no] > 0 ) {
        pthread_mutex_lock( &my_threads_mutex );
          if ( no_free_threads > 0 ) {
            no_free_threads--;
            for ( got_it = MAXCONNECTIONS ; got_it < MAXTHREADS ; got_it++ )
                if ( connection_no[got_it] == -1 ) { /*** free ***/
                    connection_no[got_it] = -2; /*** reserved ***/
                    break;
                  }
          }
        pthread_mutex_unlock( &my_threads_mutex );
        if ( got_it > 0 ) {
            (no_threads[conn_no])++;
            return got_it;
          }
        /*** else wait for all previously allocated threads to terminate ***/
        join_all_threads(conn_no);
      }
    /*** else ( no_threads[conn_no] == 0 ) ***/
    no_threads[conn_no] = 1;
    pthread_mutex_lock( &my_threads_mutex );
      for ( got_it = MAXCONNECTIONS ; got_it < MAXTHREADS ; got_it++ )
        if ( connection_no[got_it] == -1 ) { /*** free ***/
            	connection_no[got_it] = -2; /*** reserved ***/
            	pthread_mutex_unlock( &my_threads_mutex );
		return got_it;
          }
    
}


void join_all_threads(int conn_no) {

/*** TO BE DONE 1.1 START ***/
/*** implement this function ***/

	pthread_join(*(to_join[conn_no]),NULL);	
	
	connection_no[conn_no]=-1;
	
	no_threads[conn_no]--;


}

void join_prev_thread(int thrd_no) {

/*** TO BE DONE 1.1 START ***/
/*** implement this function ***/

	if(to_join[thrd_no]==NULL)	return;

	else{
		pthread_join( *(to_join[thrd_no]),NULL);		
		pthread_mutex_lock( &my_threads_mutex );
	
		no_free_threads++;		
		connection_no[to_join[thrd_no]-thread_id]=-1;			

		pthread_mutex_unlock( &my_threads_mutex );
		(no_threads[thrd_no])--;			
	}
/*** TO BE DONE 1.1 END ***/

}

void * my_resp_thread( void * vp ) {
    int my_thread_no = ((int*)vp) - connection_no;
    int my_conn_no = *((int*)vp);

#ifdef DEBUG
    printf(" ... my_resp_thread() thread_no=%1d, conn_no=%1d\n",my_thread_no,my_conn_no);
#endif
    send_response(client_socket[my_conn_no],
                  thread_params[my_thread_no-MAXCONNECTIONS].ap_answer_no,
                  thread_params[my_thread_no-MAXCONNECTIONS].ap_http1_0,
                  my_thread_no,
                  thread_params[my_thread_no-MAXCONNECTIONS].ap_filename,
                  thread_params[my_thread_no-MAXCONNECTIONS].ap_stat);

#ifdef DEBUG
    printf(" ... my_resp_thread() freeing filename and stat\n");
#endif

    if ( (thread_params[my_thread_no-MAXCONNECTIONS].ap_filename) != NULL )
        	free((void*)thread_params[my_thread_no-MAXCONNECTIONS].ap_filename);



    if ( (thread_params[my_thread_no-MAXCONNECTIONS].ap_stat) != NULL )
        	free((void*)thread_params[my_thread_no-MAXCONNECTIONS].ap_stat);

}


#else /* #ifndef HTTP_1_1 */

  pthread_t thread_id[MAXCONNECTIONS];
  int connection_no[MAXCONNECTIONS];

#endif /* ifdef HTTP_1_1 */


void * client_connection_thread( void * vp ) {
    int clientSocketFD, count;
    struct sockaddr_storage clientAddr;
    socklen_t addrSize;
#ifdef HTTP_1_1
    int my_conn_no = *((int*)vp);

/*** TO BE DONE 1.1 START ***/
/*** properly initialize to_join[my_conn_no] ***/

	to_join[my_conn_no]=NULL;

/*** TO BE DONE 1.1 END ***/

#endif

    for ( ; ; ) {
        addrSize = sizeof(clientAddr);
        pthread_mutex_lock( &my_accept_mutex );
        if ((clientSocketFD=accept(listenSocketFD, (struct sockaddr*)&clientAddr,&addrSize)) == -1) {
            fail_errno("Cannot accept client connection");
        }
        pthread_mutex_unlock( &my_accept_mutex );

#ifdef HTTP_1_1
        client_socket[my_conn_no] = clientSocketFD;
#endif
        /*** Print information about the connected client ***/
        char str[INET_ADDRSTRLEN];
        struct sockaddr_in *ipv4 = (struct sockaddr_in *) &clientAddr;
        printf("Accepted connection from %s \n",
               inet_ntop(AF_INET, &(ipv4->sin_addr), str, 20));

        /*** Get HTTP request and send HTTP response on the socket ***/
          manage_http_requests(clientSocketFD
#ifdef HTTP_1_1
                            ,my_conn_no
#endif
                            );

    }
}


char * get_mime_type( char * filename ) {
    char * mime_t = NULL;
    int nc = 0;
    size_t sz = 0;

    if ( (nc = strlen(filename)) > 4 && strcmp(filename+nc-4,".css") == 0 ) {
        mime_t = (char*)check_malloc((void*)strdup("text/css;"));
        return mime_t;
    }
#ifdef DEBUG
    printf("      ... get_mime_type(%s): was not .css\n",filename);
#endif

/*** TO BE DONE 1.0 START ***/
/*** what is missing here ? ;-) ***/

	pthread_mutex_lock(&my_mime_mutex);

/*** TO BE DONE 1.0 END ***/

    fprintf(mreq_f,"%s\n",filename);
    fflush(mreq_f);
#ifdef DEBUG
    printf("      ... get_mime_type(%s): printed filename on mreq_f\n",filename);
#endif
    while ( (nc = getline(&mime_t,&sz,mansw_f)) < 0 );
#ifdef DEBUG
    printf("      ... get_mime_type(%s): got line of %1d characters\n",filename,nc);
#endif

/*** TO BE DONE 1.0 START ***/
/*** what is missing here ? ;-) ***/

	/*if(pclose(mreq_f)==-1){
		fail_errno("errore pclose()");
	}*/
	pthread_mutex_unlock(&my_mime_mutex);
   
/*** TO BE DONE 1.0 END ***/

    if ( nc < 1 )
        return NULL;
#ifdef DEBUG
    printf("      ... get_mime_type(%s): got answer %s\n",filename,mime_t);
#endif
    if ( mime_t[(--nc)] == '\n' )
        mime_t[nc] = '\0';
    return mime_t;
}


#ifdef HTTP_1_1

void send_resp_thread( int out_socket, int http_response, 
                       int http10, int my_conn_no, int my_thrd,
                       char * filename, struct stat * stat_p) {
    int * myip = connection_no+my_thrd, pindex = my_thrd-MAXCONNECTIONS;

#ifdef DEBUG
    printf(" ... send_resp_thread(): pindex=%1d\n",pindex);
#endif
    thread_params[pindex].ap_answer_no = http_response;
    thread_params[pindex].ap_http1_0 = http10;
    if ( filename == NULL )
        thread_params[pindex].ap_filename = NULL;
      else
        thread_params[pindex].ap_filename = (char*)check_malloc((void*)strdup(filename));
    thread_params[pindex].ap_stat = stat_p;
    connection_no[my_thrd] = my_conn_no;
#ifdef DEBUG
    printf(" ... send_resp_thread(): parameters set, conn_no=%1d\n",my_conn_no);
#endif

/*** TO BE DONE 1.1 START ***/

/*** properly update to_join[my_thrd] and to_join[my_conn_no] ***/

	to_join[my_thrd] = to_join[my_conn_no];
	to_join[my_conn_no] = &(thread_id[my_thrd]);


/*** TO BE DONE 1.1 END ***/

    if ( pthread_create(thread_id+my_thrd,NULL,my_resp_thread,(void*)(myip)) )
        fail_errno("Could not create response thread");
#ifdef DEBUG
    printf(" ... send_resp_thread(): new thread created\n");
#endif

}

#endif
