#ifndef __IO_GUARD__
#define __IO_GUARD__

#include "colors.h"

/* std */
#include <stdio.h>
#include <stdlib.h>

/* needed for inflateblock */
#include <assert.h>
#include "zlib.h"
/* zlib defs*/
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
	#include <fcntl.h>
	#include <io.h>
	#define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
	#define SET_BINARY_MODE(file)
#endif
#define CHUNK 16384
/* */

/* prototypes */
int sameblock( unsigned char *a, unsigned char *b, int n);
int samestring( char *a, char *b);
void strcopy( char *a, char *b );
void lowercase( char *str );
void lowercaseover( char *str, int n );
void getmsg( char * str, unsigned char *msg , int n);
void setproid( char *name , int n );
int objsubtype ( int n );
int read_BE_int( unsigned char *pointer );
void write_BE_int( int k, unsigned char *out );
int inflateblock(unsigned char *packed, int psize, unsigned char *real, int rsize);
int loadfromfile(unsigned char **pointer, char *filename);
int loadfromdat2(unsigned char **pointer, char *filename, char *datname );
int savetofile(unsigned char *pointer, int size, char *filename);
int isf2sav( char *filename);
#endif
