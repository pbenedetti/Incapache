/* 
 * incApache_mime.c: implementazione del processo ausiliario che lancia
 *                   il web server del corso di SET
 *
 * versione 1.0 del 02/12/2012
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

/*
#define DEBUG
*/

#include "incApache.h"


main(int argc, char ** argv, char ** envp) {
  char * mycp = NULL, * exec_path, * my_ex_name = EXECUT1;
  int mypid, myport, mime_req[2], mime_answ[2], len;
  size_t mysz;
  DIR * mydir;
  FILE * mreq_f, * mansw_f;

  if ( (argc != 3) || (sscanf(argv[1],"%d",&myport) < 1) || ((myport < 1024)&&(myport != 80)) ||
       (myport >= 64*1024) || ((mydir = opendir(mycp = argv[2])) == NULL) )
        fail("Incorrect syntax. Use: incapache PORTNUM WWWROOT\n");
  (void)closedir(mydir);

  if ( strncmp(argv[0],"./",2) == 0 || strncmp(argv[0],"../",3) == 0 ) {
    /*** relative path name ***/
      exec_path = getcwd(NULL,0);
      exec_path = (char*)check_malloc(realloc((void*)exec_path,strlen(exec_path)+strlen(argv[0])+2));
      exec_path = strcat(exec_path,"/");
      exec_path = strcat(exec_path,argv[0]);
  } else /*** absolute path name ***/
      exec_path = (char*)check_malloc((void*)strdup(argv[0]));

  if ( chdir(mycp) < 0 )
      fail_errno("Could not chdir to WWWROOT");
  if ( pipe(mime_req) < 0 )
      fail_errno("Could not create mime_req[2] pipe");
  if ( pipe(mime_answ) < 0 )
      fail_errno("Could not create mime_answ[2] pipe");
#ifdef DEBUG
  fprintf(stderr,"%s opened mime_req %1d %1d and mime_answ %1d %1d before forking\n",
          my_ex_name,mime_req[0],mime_req[1],mime_answ[0],mime_answ[1]);
#endif
  if ( (mypid = fork()) < 0 )
      fail_errno("Could not fork()");

  if ( mypid == 0 ) { /*** this is the real incApache process ***/
      char * myexe = EXECUT2;
      char myintstr[MAXINTSTR];
      int myUID = getuid();

      exec_path = dirname(exec_path);
      exec_path = (char*)check_malloc(realloc((void*)exec_path,strlen(exec_path)+strlen(myexe)+2));
      sprintf(exec_path,"%s/%s",exec_path,myexe);
      mycp = (char*)check_malloc((void*)strdup(argv[1]));
      argv = (char**)check_malloc(malloc(6*sizeof(char*)));
      argv[0] = myexe;
#ifdef DEBUG
      fprintf(stderr,"%s is going to execve(%s)\n",my_ex_name,exec_path);
#endif
      argv[1] = mycp;
      sprintf(myintstr,"%d",myUID);
      argv[2] = (char*)check_malloc((void*)strdup(myintstr));
      sprintf(myintstr,"%d",mime_req[PIPE_WRITE_END]);
      if ( close(mime_req[PIPE_READ_END]) < 0 )
          fail_errno("Could not close mime_req[PIPE_READ_END]");
      argv[3] = (char*)check_malloc((void*)strdup(myintstr));
      sprintf(myintstr,"%d",mime_answ[PIPE_READ_END]);
      argv[4] = (char*)check_malloc((void*)strdup(myintstr));
      if ( close(mime_answ[PIPE_WRITE_END]) < 0 )
          fail_errno("Could not close mime_answ[PIPE_WRITE_END]");
      argv[5] = NULL;
      if ( execve(exec_path,argv,envp) < 0 )
          fail_errno("Could not execve()");
  }

  /*** else ( mypid > 0 ) : this is the MIME type computation process ***/

  free((void*)exec_path);
  if ( close(mime_answ[PIPE_READ_END]) < 0 )
      fail_errno("Could not close mime_answ[PIPE_READ_END]");
  if ( close(mime_req[PIPE_WRITE_END]) < 0 )
      fail_errno("Could not close mime_req[PIPE_WRITE_END]");
  if ( (mreq_f = fdopen(mime_req[PIPE_READ_END],"r")) == NULL )
      fail_errno("Could not open mreq_f stream in r mode");
  if ( (mansw_f = fdopen(mime_answ[PIPE_WRITE_END],"w")) == NULL )
      fail_errno("Could not open mansw_f stream in w mode");
#ifdef DEBUG
  fprintf(stderr," ... %s opened files %1d for r and %1d for w; starting mime loop\n",
          my_ex_name,mime_req[PIPE_READ_END],mime_answ[PIPE_WRITE_END]);
#endif
  for ( mysz = 0, mycp = NULL ; (len = getline(&mycp,&mysz,mreq_f)) > 0 ; ) {
#ifdef DEBUG
      fprintf(stderr,"   ... %s mime loop: read line of %1d characters (%s)\n",
              my_ex_name,len,mycp);
#endif
      char * mycmnd = "file -i ";
      int mylen = strlen(mycp);
      char * string = (char*)check_malloc(malloc(strlen(mycmnd)+mylen));
      FILE * file_answ;
      char * answer;

      mycp[mylen-1] = '\0'; /*** remove terminating '\n' ***/
      string = strcpy(string,mycmnd);
      string = strcat(string,mycp);
#ifdef DEBUG
      fprintf(stderr,"     ... %s mime loop: popen(%s)\n",my_ex_name,string);
#endif
      if ( (file_answ = popen(string,"r")) == NULL )
          fail("Could not popen()");
      free((void*)string);
      string = NULL; mylen = 0;
      if ( getline(&string,&mylen,file_answ) < 1 )
          fail("Could not read answer from popen()");
      pclose(file_answ);
#ifdef DEBUG
      fprintf(stderr,"       ... %s mime loop: got string (%s)\n",my_ex_name,string);
#endif
      if ( (answer = strtok(string,": ")) == NULL || (answer = strtok(NULL,"\n")) == NULL )
          fail("Could not read MIME type");
      fprintf(mansw_f,"%s\n",answer);
#ifdef DEBUG
      fprintf(stderr,"         ... %s mime loop: printing answer (%s)\n",
              my_ex_name,answer);
#endif
      fflush(mansw_f);
      free((void*)string);
  }
#ifdef DEBUG
  fprintf(stderr,"  ... %s mime loop ended with value %1d\n",my_ex_name,len);
#endif

}
