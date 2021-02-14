#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "offset.h"

/* TODO : clean code */

int main( int argc, char *argv[] ) {

	unsigned int 	i,n,size;
	int		trait_1=-1,trait_2=-1;
	unsigned char	*save;
	ZONE		savezone;
	unsigned char	*msgtraits;
	char		*message;

	/* load messages */
	loadfromdat2(&msgtraits,"text/english/game/trait.msg","MASTER.DAT");
	/* create message buffer */
	message=malloc(1024);
	if ( message == NULL ) {
		fprintf(stdout,"Can't allocate 1024 bytes.\n");
		return 1;
	}

	if ( argc < 2 ) {
		fprintf(stdout,"Usage :\n");
		fprintf(stdout,"  traits\n");
		fprintf(stdout,"    Lists traits with id.\n");
		fprintf(stdout,"  traits [save.dat]\n");
		fprintf(stdout,"    Shows traits.\n");
		fprintf(stdout,"  traits [save.dat] [trait_id] [trait_id]\n");
		fprintf(stdout,"    Set traits.\n");
		fprintf(stdout,"List of traits :\n");
		for(i=0;i<16;i++){
			fprintf(stdout,"  %2i ",i);
			getmsg(message,msgtraits,i+100);
			fprintf(stdout,"%s",message);		
			fprintf(stdout,"\n");
		}
		return 0;
	}

	if ( argc == 3) {
		fprintf(stdout,_WARN_);
		fprintf(stdout,"You must specify two values to set.\n");
	}
	if ( argc == 4) {
		trait_1 = atoi(argv[2]);
		trait_2 = atoi(argv[3]);
	}

	if ( trait_1 > 15) {
		fprintf(stderr,_FAIL_);
		fprintf(stderr,"Trait ID can't be above 15. Aborting\n");
		return 1;
	}
	if ( trait_2 > 15) {
		fprintf(stderr,_FAIL_);
		fprintf(stderr,"Trait ID can't be above 15. Aborting\n");
		return 1;
	}

	if ( isf2sav( argv[1] ) == 0 ) {
		fprintf(stderr,_FAIL_);
		fprintf(stderr,"%s is not a Fallout 2 savegame.\n",argv[1]);
		return 1;
	}

	/* load SAVE.DAT */
	size=loadfromfile(&save,argv[1]);
	if (size == -1) {
		free(msgtraits);
		free(message);
		return 1;
	}
	fprintf(stdout,_INFO_);
	fprintf(stdout,"%i bytes loaded\n",size);


	/* init zone */
	savezone=getzone(save);
	if (savezone.traits == NULL) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"null pointer.\n");
		free(savezone.start);
		return 1;
	}

	/* jump to name */
	save=savezone.start+0x1d;
	fprintf(stdout,"Name    : %s\n",save);	

	/* jump to traits */
	save=savezone.traits;

	/* TRAITS */
	if (argc == 4) {
		save[3]=(char) trait_1;
		/* goto trait_2 */
		save+=4;
		save[3]=(char) trait_2;
		/* go back to trait_1 */
		save-=4;
	}	

	for(i=0;i<2;i++){
		n = read_BE_int(save);
		save+=4;
		getmsg(message,msgtraits,n+100);
		fprintf(stdout,"Trait   :  %2i %s\n",n,message);
		if ( n > 15) {
			fprintf(stderr,_FAIL_);
			fprintf(stderr,"Trait ID can't be above 15. Aborting\n");
			free(savezone.start);
			free(msgtraits);
			free(message);
			return 1;
		}
	}

	/* save to file */
	if ( trait_2 != -1){
		n=savetofile(savezone.start,size,argv[1]);
		fprintf(stdout,_INFO_);
		fprintf(stdout,"%i bytes written\n",n);
	}

	/* clean exit */
	free(savezone.start);
	free(msgtraits);
	free(message);
	return 0;

}
