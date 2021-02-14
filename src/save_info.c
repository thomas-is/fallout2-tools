#include <stdio.h>
#include <stdlib.h>
#include "colors.h"
#include "io.h"
#include "offset.h"


/*
	TODO

	Inventory	Specify if item is equiped
			Show radiation and poison level

	Skills		Show them !

	Exp		Show both levels (real & interface)

	Kills		There should be a message file for that in MASTER.DAT
	
*/


int main( int argc, char *argv[] ) {

	unsigned int 	i,n,q,id,count,size,allnull=1;
	int		scriptid;
	int		type;

	/* buffer */
	unsigned char	*save;
	/* zone */
	ZONE		savezone;
	/* message buffers */
	unsigned char	*msgperks;
	unsigned char	*msgitems;
	unsigned char	*msgtraits;
	unsigned char	*msgskills;
	char		*message;


	if ( argc < 1 ) {
		fprintf(stderr,"Usage : save_info [save.dat]\n");
		return -1;
	}

	if ( isf2sav( argv[1] ) == 0 ) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"%s is not a Fallout 2 savegame.\n",argv[1]);
		return 1;
	}


	/* load messages */
	loadfromdat2(&msgperks,"text/english/game/perk.msg","MASTER.DAT");
	loadfromdat2(&msgitems,"text/english/game/pro_item.msg","MASTER.DAT");
	loadfromdat2(&msgtraits,"text/english/game/trait.msg","MASTER.DAT");
	loadfromdat2(&msgskills,"text/english/game/skill.msg","MASTER.DAT");
	/* create message buffer (size of 1024 is arbitrary) */
	message=malloc(1024);
	if ( message == NULL ) {
		fprintf(stdout,"Can't allocate 1024 bytes.\n");
		return 1;
	}
	/* load SAVE.DAT */
	size=loadfromfile(&save,argv[1]);
	if (size == -1) return 1;
	fprintf(stdout,_INFO_);
	fprintf(stdout,"%i bytes loaded from %s\n",size,argv[1]);

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
		free(msgperks);
		free(msgitems);
		free(msgtraits);
		free(msgskills);
		free(message);
		return 1;
	}

	/* jump to radiation level */
/*
	save=savezone.inventory+0x78;
	n = read_BE_int(save);	
	fprintf(stdout,"Radiation : %3i\n",n);
*/
	/* jump to poison level */
/*
	save=savezone.inventory+0x78;
	n = read_BE_int(save);	
	fprintf(stdout,"Poison    : %3i\n",n);
*/

	/* jump to number of items in inventory */
	save=savezone.inventory+0x48;
	count = read_BE_int(save);	
	save+=4;
	fprintf(stdout,"%i items in inventory.\n",count);
	/* jump to objects list */
	save+=0x34;
	for(i=0;i<count;i++){
		/* quantity */
		q = read_BE_int(save);	
		/* jump to object id */
		save+=0x30;
		id = (unsigned int)read_BE_int(save);	
		type = objsubtype (id);
		/* jump to script id */
		save+=0x14;
		scriptid = (int)read_BE_int(save);
		/* jump to optional field */
		save+=0x18;
		/* skip specific field for Weapon, Ammo, Misc, Key */
		if ( type > 2 ) save+=4;
		/* skip specific field for Weapon */
		if (type == 3)  save+=4;

		if (type == -1) {
			free(savezone.start);
			free(msgperks);
			free(msgitems);
			free(msgtraits);
			free(msgskills);
			free(message);
			return 1;
		}

		fprintf(stdout,"%3i : ",i);
		getmsg(message,msgitems,id*100);
		fprintf(stdout,"[%3i] %-30s ",id,message);
		if (type == 0) fprintf(stdout,"(Armor)    ");
		if (type == 1) fprintf(stdout,"(Container)");
		if (type == 2) fprintf(stdout,"(Drug)     ");
		if (type == 3) fprintf(stdout,"(Weapon)   ");
		if (type == 4) fprintf(stdout,"(Ammo)     ");
		if (type == 5) fprintf(stdout,"(Misc)     ");
		if (type == 6) fprintf(stdout,"(Key)      ");
		if (type > 6) fprintf(stdout,"(?)       ");
		fprintf(stdout," x %6i",q);
		fprintf(stdout," %6x\n",scriptid);

	}	

	/* STATS */
	save=savezone.stats;
	if (savezone.stats == NULL) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"STATS : null pointer.\n");
		free(savezone.start);
		return 1;
	}
	n = read_BE_int(save);
	save+=4;
/*
	fprintf(stdout,"Unknown   :  %2i ",n);
*/
	n = read_BE_int(save);
	save+=4;
/*
	fprintf(stdout,"%2i\n",n);
*/
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"SPECIAL   :  %2i ",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"%2i ",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"%2i ",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"%2i ",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"%2i ",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"%2i ",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"%2i\n",n);
	n = read_BE_int(save);
	save+=4;
/*
	fprintf(stdout,"HP AP AC  :  %2i ",n);
*/
	n = read_BE_int(save);
	save+=4;
/*
	fprintf(stdout,"%2i ",n);
*/
	n = read_BE_int(save);
	save+=4;
/*
	fprintf(stdout,"%2i\n",n);
*/
	/* jump to bonuses */
/*	save=savezone.stats+0x94;
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  Bonus ST: %2i\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  Bonus PE: %2i\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  Bonus EN: %2i\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  Bonus CH: %2i\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  Bonus IN: %2i\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  Bonus AG: %2i\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  Bonus LK: %2i\n",n);
*/

	/* TAGS */
	save=savezone.tags;
	if (savezone.tags == NULL) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"TAGS : null pointer.\n");
		free(savezone.start);
		return 1;
	}
	for (i=0;i<4;i++){
		n = read_BE_int(save);
		save+=4;
		fprintf(stdout,"Tag       :  %2i ",n);
		if ( n == -1) 	fprintf(stdout,"UNUSED\n");
		else {
			getmsg(message,msgskills,n+100);
			fprintf(stdout,"%s\n",message);
		}
	}

	/* PERKS */
	save=savezone.perks;
	if (savezone.perks == NULL) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"PERKS : null pointer.\n");
		free(savezone.start);
		return 1;
	}
	for (i=0;i<178;i++){
		n = read_BE_int(save);
		save+=4;
		if ( n> 0) {
			allnull=0;
			fprintf(stdout,"Perk      : %3i (%i) ",i,n);
			getmsg(message,msgperks,i+101);
			fprintf(stdout,"%s",message);		
			fprintf(stdout,"\n");
		}
		if ( allnull == 0) {
			fprintf(stdout,"Perk      : [none]\n");
		}
	}

	/* TRAITS */
	save=savezone.traits;
	if (savezone.traits == NULL) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"TRAITS : null pointer.\n");
		free(savezone.start);
		return 1;
	}
	for(i=0;i<2;i++){
		n = read_BE_int(save);
		save+=4;
		getmsg(message,msgtraits,n+100);
		fprintf(stdout,"Trait     :  %2i %s\n",n,message);
/*		if ( n > 15) {
			fprintf(stdout,_FAIL_);
			fprintf(stderr,"Trait ID can't be above 15. Aborting\n");
			free(savezone.start);
			free(msgtraits);
			free(message);
			return 1;
		}
*/	}


	/* EXPERIENCE */
	save=savezone.exp;
	if (savezone.exp == NULL) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"EXP : null pointer.\n");
		free(savezone.start);
		return 1;
	}
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"Skill pts :  %2i\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"Level     :  %2i\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"Exp       :  %6i\n",n);
	save+=8; /* unknown */




	/* KILL COUNT */
/*	save=savezone.kills;
	fprintf(stdout,"* Kill count\n");
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Men\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Women\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Children\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Super Mutants\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Ghouls\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Brahims\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Radscorpions\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Rats\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Floaters\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Centaurs\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Robots\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Dogs\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Mantis\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Deathclaws\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Plants\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Geckos\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Aliens\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Giants Ants\n",n);
	n = read_BE_int(save);
	save+=4;
	fprintf(stdout,"  %4i Big Bad Boss\n",n);

*/


	free(savezone.start);
	free(msgperks);
	free(msgitems);
	free(msgtraits);
	free(msgskills);
	free(message);
	return 0;

}
