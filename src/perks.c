#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "offset.h"

int main( int argc, char *argv[] ) {

	unsigned int 	i,n,size;
	int		perkid,perkvalue,allnull=1;
	unsigned char	*save;
	ZONE		savezone;
	unsigned char	*msgperks;
	char		*message;


	/* load messages */
	loadfromdat2(&msgperks,"text/english/game/perk.msg","MASTER.DAT");
	/* create message buffer */
	message=malloc(1024);
	if ( message == NULL ) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"Can't allocate 1024 bytes.\n");
		return 1;
	}

	if ( argc < 2 ) {
		fprintf(stdout,"Usage :\n");
		fprintf(stdout,"  perks\n");
		fprintf(stdout,"    Lists perks with id.\n");
		fprintf(stdout,"  perks [save.dat]\n");
		fprintf(stdout,"    Shows perks state.\n");
		fprintf(stdout,"  perkss [save.dat] [perk_id] [perk_level]\n");
		fprintf(stdout,"    Set perk state.\n");
		fprintf(stdout,"List of perks :\n");
		fprintf(stdout,"   ID Name\n");
		for(i=0;i<0x77;i++){
			fprintf(stdout,"  %3i ",i);
			getmsg(message,msgperks,i+101);
			fprintf(stdout,"%s",message);		
			fprintf(stdout,"\n");
		}
		return 0;
	}

	if ( isf2sav( argv[1] ) == 0 ) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"%s is not a Fallout 2 savegame.\n",argv[1]);
		return 1;
	} 
/*
		fprintf(stdout,_WARN_);
		fprintf(stdout,"OK\n");
*/	

	perkid = -1;
	perkvalue = 0;
	if ( argc == 4) {
		perkid = atoi(argv[2]);
		perkvalue = atoi(argv[3]);
	}

	/* open save.dat */
	size=loadfromfile(&save,argv[1]);
	fprintf(stdout,_INFO_);
	fprintf(stdout,"%i bytes loaded\n",size);

	/* init zone */
	savezone=getzone(save);

	if (savezone.perks == NULL) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"null pointer.\n");
		free(savezone.start);
		return 1;
	}

	/* jump to name */
	save=savezone.start+0x1d;
	fprintf(stdout,"Name    : %s\n",save);	

	/* jump to perks */
	save=savezone.perks;


	/* PERKS */
	for (i=0;i<178;i++){
		if ( i == perkid ){
			save[3] = (unsigned char) perkvalue;
		}
		n = read_BE_int(save);
		save+=4;
		if ( n> 0) {
			allnull=0;
			fprintf(stdout,"Perk    : %3i (%i) ",i,n);
			getmsg(message,msgperks,i+101);
			fprintf(stdout,"%s",message);		
			fprintf(stdout,"\n");
		}
	}
	if ( allnull == 1) fprintf(stdout,"  No perks.\n");


	if (perkid != -1){
		n=savetofile(savezone.start,size,argv[1]);
		fprintf(stdout,_INFO_);
		fprintf(stdout,"%i bytes written\n",n);
	}


	free(savezone.start);
	return 0;
}




