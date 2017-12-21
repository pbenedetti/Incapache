/* 
 * incApache_main.c: implementazione del main per il web server del corso di SET
 *
 * versione 1.0 del 03/12/2012
 *
 * Programma sviluppato a supporto del laboratorio di
 * Sistemi di Elaborazione e Trasmissione del corso di laurea
 * in Informatica classe L-31 presso l'Universita` degli Studi di
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
#define DEBUG



#include "incApache.h"


int listenSocketFD;
FILE * mreq_f, * mansw_f;


main(int argc, char ** argv, char ** envp) {
  char * mycp = NULL, * my_exec_name = EXECUT2;
  int i, myport, myUID, mime_req_fd, mime_answ_fd;
  struct addrinfo hints, *serverAddr;

  if ( argc != 5 || sscanf(argv[1],"%d",&myport) < 1 ||
       ((myport < 1024)&&(myport != 80)) || myport >= 64*1024 ||
       sscanf(argv[2],"%d",&myUID) < 1 ||
       sscanf(argv[3],"%d",&mime_req_fd) < 1 ||
       sscanf(argv[4],"%d",&mime_answ_fd) < 1 )
        fail("Incorrect argv. Expecting: PORTNO UIDNO MIMEREQFD MIMEANSWFD\n");


#ifdef DEBUG
  fprintf(stderr," ... %s: mime_req_fd=%1d, mime_answ_fd=%1d\n",
          my_exec_name,mime_req_fd,mime_answ_fd);
#endif

/*** TO BE DONE 1.0 START ***/
	/*** initialize mreq_f and mansw_f ***/
	
	mreq_f=fdopen(mime_req_fd, "w");
	mansw_f=fdopen(mime_answ_fd, "r");

	/***  assign current directory path to mycp and execute chroot() on it ***/
	char* tmp=(char*)get_current_dir_name();
	mycp=(char*)check_malloc(malloc(strlen(tmp)+1));
	mycp=strcpy(mycp,tmp);
	printf("mycp= %s\n", mycp);
	if(chroot(mycp) == -1){
		fail_errno("Cannot chroot");
	}

	/*** Specify the listening socket options ***/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = IPPROTO_TCP;

	/*** Retrieve the information required for opening a server socket ***/
	int gai_rv;
	if ((gai_rv = getaddrinfo(NULL, argv[1], &hints, &serverAddr)) != 0)
		fail((char *) gai_strerror(gai_rv));

	/*** Open, bind and listen on the server socket ***/
	if ((listenSocketFD = socket(serverAddr->ai_family, serverAddr->ai_socktype, serverAddr->ai_protocol)) == -1)
		fail("Cannot allocate socket descriptor");
	if (bind(listenSocketFD, serverAddr->ai_addr, serverAddr->ai_addrlen) == -1) {
		close(listenSocketFD);
		fail_errno("Cannot bind socket");
	}
	if (listen(listenSocketFD, BACKLOG) == -1) {
		close(listenSocketFD);
		fail_errno("Cannot listen on socket");
	}

	/***  call setuid() on myUID ***/
	if(setuid(myUID) != 0){
		printf("errore setuid()\n");
	}
/*** TO BE DONE 1.0 END ***/

  printf("%s server: listening on port %d with WWW Root set to %s\n\n", my_exec_name,myport, mycp);
  printf("Waiting for connections ...\n");

#ifdef HTTP_1_1
  for ( i = MAXCONNECTIONS ; i < MAXTHREADS ; i++ ) {
      connection_no[i] = -1;
  }
#endif
										
  for ( i = 0 ; i < MAXCONNECTIONS ; i++ ) {
      connection_no[i] = i;
										
#ifdef HTTP_1_1
      no_threads[i] = 0;
#endif

/*** TO BE DONE 1.0 START ***/
/*** create the i-th thread executing client_connection_thread() function ***/

	if(pthread_create(&thread_id[i], NULL, client_connection_thread, &connection_no[i] )!=0)
		fail_errno("pthread_create fail");

/*** TO BE DONE 1.0 END ***/
										
  }

  for ( i = 0 ; i < MAXCONNECTIONS ; i++ )
      if ( pthread_join(thread_id[i],NULL) ) 
          fail_errno("Could not join thread");
  close(listenSocketFD);
}
