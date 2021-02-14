#include <stdio.h>
#include <stdlib.h>
#include "io.h"

int main( int argc, char *argv[] ) {

	FILE 			*file;
	unsigned long int	size=0;
	unsigned char		*out=NULL;

	if ( argc < 2 ) {
		fprintf(stderr,"Too few arguments.\n");
		fprintf(stderr,"Usage : dat2_dump [file]\n");
		return 1;
	}

	
	size=loadfromdat2(&out,argv[1],"MASTER.DAT");

	if ( size == 0 ) {
		fprintf(stderr,"%s not found.\n",argv[1]);
		free(out);
		return 1;
	}

	/* dump */
	file = fopen ( "dump", "wb" );	
	fwrite ( out, 1, size, file );
	fclose(file);
	/* clean */
	free(out);

	return 0;
}
