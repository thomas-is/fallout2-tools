#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "offset.h"

int main( int argc, char *argv[] ) {

	unsigned int 	n,size;
	int		bonuscriticalchance=-1,bonuscriticalroll=-1;
	unsigned char	*save;
	char		*filename=NULL;
	ZONE		savezone;


	if ( argc < 2 ) {
		fprintf(stdout,"\nUsage :\n");
		fprintf(stdout,"  critical [save.dat] [bonus_crit_chance] [bonus_crit_roll]\n\n");
		return 1;
	}

	if ( argc >= 2 ) filename=argv[1];
	if ( argc == 3 ) bonuscriticalchance = atoi(argv[2]);
	if ( argc == 4 ) {
		bonuscriticalchance = atoi(argv[2]);
		bonuscriticalroll = atoi(argv[3]);
	}

	if ( isf2sav( argv[1] ) == 0 ) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"%s is not a Fallout 2 savegame.\n",argv[1]);
		return 1;
	}

	/* open save.dat */
	size = loadfromfile(&save,argv[1]);
	fprintf(stdout,_INFO_);
	fprintf(stdout,"%i bytes loaded\n",size);

	/* init zone */
	savezone = getzone(save);

	if (savezone.stats == NULL) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"null pointer.\n");
		free(savezone.start);
		return 1;
	}

	/* Show name */
	fprintf(stdout,"Name    : %s\n",savezone.start+0x1d);

	/* Bonus critical chance */
	/* Each level (max 3) of "More Criticals" gives a +5% here */
	save = savezone.stats+0xd0;
	if ( bonuscriticalchance != -1) write_BE_int( bonuscriticalchance, save );
	n = read_BE_int(save);
	fprintf(stdout,"Bonus Crit chance : %i\n",n);
	/* Bonus critical hit table rolls */
	/* "Better Criticals" gives a +20% here */
	save = savezone.stats+0xd4;
	if ( bonuscriticalroll != -1) write_BE_int( bonuscriticalroll, save );
	n = read_BE_int(save);
	fprintf(stdout,"Bonus Crit roll   : %i\n",n);

	if ( bonuscriticalchance != -1 ) savetofile( savezone.start, size, filename );

	free(savezone.start);
	return 0;
}




