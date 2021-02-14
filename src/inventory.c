#include <stdio.h>
#include <stdlib.h>
#include "colors.h"
#include "io.h"
#include "offset.h"


/*
	TODO

	Inventory	Specify if item is equiped
			Show radiation and poison level

*/


int main( int argc, char *argv[] ) {

	unsigned int 	i,n,q,id,count,size;
	int		type;
	int		select=-1,newid=0,newq=1,newunk=-1;
	int		unk,scriptid,ogimg,opta,optb;
	char		*filename;

	/* buffer */
	unsigned char	*save;
	/* zone */
	ZONE		savezone;
	/* message buffers */
	unsigned char	*msgitems;
	char		*message;




	if ( argc < 2 ) {
		fprintf(stderr,"Usage : inventory [save.dat] [#] [id] [quantity] (unknown) \n");
		return 1;
	}

	filename=argv[1];

	if ( argc >= 5 ) {
		select = atoi(argv[2]);
		newid = atoi(argv[3]);
		newq = atoi(argv[4]);
	}
	if ( argc == 6 ) {
		newunk = atoi(argv[5]);
	}

	if ( isf2sav( filename ) == 0 ) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"%s is not a Fallout 2 savegame.\n",argv[1]);
		return 1;
	}


	/* load messages */
	loadfromdat2(&msgitems,"text/english/game/pro_item.msg","MASTER.DAT");
	/* create message buffer (size of 1024 is arbitrary) */
	message=malloc(1024);
	if ( message == NULL ) {
		fprintf(stdout,"Can't allocate 1024 bytes.\n");
		return 1;
	}

	/* list 
	for (i=1;i<532;i++){
		getmsg(message,msgitems,i*100);
		fprintf(stdout,"%3i:%i:%s\n",i,objsubtype(i),message);
	}
	free(message);
	return 0;
	*/
	



	/* load SAVE.DAT */
	size=loadfromfile(&save,filename);
	if (size == -1) return 1;
	fprintf(stdout,_INFO_);
	fprintf(stdout,"%i bytes loaded from %s\n",size,filename);

	/* init zone */
	savezone=getzone(save);

	/* NAME */
	save=savezone.start+0x1d;
	fprintf(stdout,"Name      : %s\n",save);	

	/* INVENTORY */
	save=savezone.inventory;
	if (savezone.inventory == NULL) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"INVENTORY : null pointer.\n");
		free(savezone.start);
		return 1;
	}
	/* header = 0x4650 */
	n= read_BE_int(save);	
	save+=4;
	if ( n != 0x4650 ){
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"header mismatch (0x4650 != 0x%x)\n",n);
		free(savezone.start);
		free(msgitems);
		free(message);
		return 1;
	}
	/* jump to number of items in inventory */
	save=savezone.inventory+0x48;
	count = read_BE_int(save);	
	save+=4;
	fprintf(stdout,"%i items in inventory.\n",count);
	/* jump to objects list */
	save+=0x34;
	for(i=0;i<count;i++){
		/* static fields */
		q = read_BE_int(save);
		unk = read_BE_int(save+0x14);
		ogimg = read_BE_int(save+0x24);
		id = read_BE_int(save+0x30);
		scriptid = read_BE_int(save+0x44);
		type = objsubtype (id);

		if ( i == select ) {
			if ( type == objsubtype(newid) ) {
				write_BE_int( newq, save );
				write_BE_int( newid, save+0x30 );
				if ( newunk !=-1 ) {
					write_BE_int( newunk, save+0x14 );
				}
				q = read_BE_int(save);
				id = read_BE_int(save+0x30);
				unk = read_BE_int(save+0x14);
			}
		}

		/* jump to optional field */
		save+=0x5C;

		/* skip specific field for Weapon, Ammo, Misc, Key */
		if ( type > 2 ) {
			opta=read_BE_int(save);
			save+=4;
		}
		/* skip specific field for Weapon */
		if (type == 3)  {
			optb=read_BE_int(save);
			save+=4;
		}
		/* error handler */
		if (type == -1) {
			free(savezone.start);
			free(msgitems);
			free(message);
			return 1;
		}

		/* display */
		fprintf(stdout,"%3i : ",i);
		getmsg(message,msgitems,id*100);
		fprintf(stdout,"[%3i] %6i x %-25s ",id,q,message);
/*
		fprintf(stdout," %6i %6i ",unk,scriptid);
*/
		if (type == 0) fprintf(stdout,"(Armor)    ");
		if (type == 1) fprintf(stdout,"(Container)");
		if (type == 2) fprintf(stdout,"(Drug)     ");
		if (type == 3) {
			getmsg(message,msgitems,optb*100);
			fprintf(stdout,"(Weapon)   %3i x %s",opta,message);
		}
		if (type == 4) fprintf(stdout,"(Ammo)     %3i",opta);
		if (type == 5) fprintf(stdout,"(Misc)     [%8xh] ",opta);
		if (type == 6) fprintf(stdout,"(Key)      [%8xh] ",opta);
		if (type > 6) fprintf(stdout,"(?)       ");
		fprintf(stdout,"\n");


	}	

	if ( select != -1) savetofile(savezone.start, size, filename );

	free(savezone.start);
	free(msgitems);
	free(message);
	return 0;

}
