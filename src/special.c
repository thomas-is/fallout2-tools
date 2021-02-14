#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "offset.h"

int main( int argc, char *argv[] ) {

	int 		size,i;
	unsigned int	special[7]={0,0,0,0,0,0,0};
	unsigned char	*save;
	char		*filename=NULL;
	ZONE		savezone;


	if ( argc == 1 ) {
		fprintf(stdout,"\nUsage :\n");
		fprintf(stdout,"  special [save.dat]\n");
		fprintf(stdout,"  special [save.dat] [ST] [PE] [EN] [CH] [IN] [AG] [LK]\n\n");
		return 0;
	}

	if ( argc == 2) {
		filename = argv[1];
	}

	if ( argc == 9) {
		filename = argv[1];
		for (i=0;i<7;i++) special[i]=atoi(argv[i+2]);
	}

	if ( isf2sav( filename ) == 0 ) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"%s is not a Fallout 2 savegame.\n",filename);
		return 1;
	} 

	/* open save.dat */
	size=loadfromfile(&save,filename);
	fprintf(stdout,_INFO_);
	fprintf(stdout,"%i bytes loaded\n",size);

	/* init zone */
	savezone=getzone(save);

	if (savezone.stats == NULL) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"null pointer.\n");
		free(savezone.start);
		return 1;
	}

	/* Show name */
	fprintf(stdout,"Name    : %s\n",savezone.start+0x1d);	

	/* jump to base stats */
	save=savezone.stats+0x08;
	for (i=0;i<7;i++) {
		if ( special[i] != 0 ) write_BE_int( special[i], save);
		special[i] = read_BE_int( save );
		save+=4;
	}

	fprintf(stdout,"SPECIAL : ");
	for (i=0;i<7;i++) fprintf(stdout,"%2i ",special[i]);
	fprintf(stdout,"\n");

	if ( argc == 9 ) {
		savetofile(savezone.start,size,filename);
	}
	free(savezone.start);
	return 0;
}




