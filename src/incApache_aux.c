/* 
 * incApache_aux.c: funzioni ausiliarie per il web server
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

#include "incApache.h"

void fail_errno(char *msg) {
    perror(msg);
    exit(1);
}


void fail(char *msg) {
    fprintf(stderr,"%s\n",msg);
    exit(1);
}


void * check_malloc( void * ptr ) {
    if ( ptr == NULL )
        fail("could not malloc");
    return ptr;
}

