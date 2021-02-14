#include "offset.h"

/*
TODO : read GVARs size from vault13.gam

offset	0x7567
size 	0x04 * ngvar
 	An array of global variables (GVAR).
	NG is the number of GVARs in vault13.gam .

*/

ZONE getzone (unsigned char *pointer) {

	ZONE		zone;
	char		header[]="FALLOUT SAVE FILE";
	unsigned int 	i,n,id,count,type;
	unsigned int	ngvar,np;

	/* Hard coded params */
	/* Number of GVARS in vault13.gam */
	ngvar=791;
	/* Size of party in party.txt */
	/* FIXME : should be 28 or 29 */
	np=27; 

	/* init all zones to NULL */
	zone.start = NULL;
	zone.inventory = NULL;
	zone.stats = NULL;
	zone.kills = NULL;
	zone.tags = NULL;
	zone.perks = NULL;
	zone.exp = NULL;
	zone.traits = NULL;

	/* check SAV header */
	if ( samestring( (char *) pointer,header) == 0) {
		fprintf(stderr,_FAIL_);
		fprintf(stderr,"wrong header.\n");
		return zone;
	}

	/* HEADER (size 0x7567) */
	zone.start = pointer;
	/* goto GVARS */
	pointer = zone.start+0x7567;

	/* GVARS (size 4*ngvar) */
	/* goto MAPS */
	pointer += 4*ngvar;

	/* MAPS (size variable) */
	/* read number of open maps */
	n = read_BE_int(pointer);
	pointer += 4;
	/* skip n null-terminated ascii strings */
	for (i=0;i<n;i++){
		while( pointer[0] != 0x00 ) pointer++;
		pointer++;
	}
	/* skip unknown field */
	pointer += 4;

	/* GVARs copy (size 4*ngvar) */
	/* skip GVARs copy */
	pointer += 4*ngvar;

	/* INVENTORY (size variable) */
	zone.inventory = pointer;
	n = read_BE_int(pointer);
	if ( n != 0x4650 ){
		fprintf(stderr,_FAIL_);
		fprintf(stderr,"Inventory header mismatch : %x.\n",n);
		return zone;
	}
	/* jump to number of items in inventory */
	pointer += 0x48;
	count = read_BE_int(pointer);
	/* jump to objects list */
	pointer += 0x38;
	for(i=0;i<count;i++){
		/* jump to objectid */
		pointer += 0x30;
		id = (unsigned int)read_BE_int(pointer);	
		/* jump to optional field */
		pointer += 0x2c;
		type = objsubtype (id);
		/* skip specific field for Weapon, Ammo, Misc, Key */
		if ( type > 2 ) pointer += 4;
		/* skip specific field for Weapon */
		if (type == 3)  pointer += 4;
		if (type == -1) {
			fprintf(stderr,_FAIL_);
			fprintf(stderr,"wrong subtype.\n");
			return zone;
		}
	}
	/* FIXME : needed but don't know why */
	pointer +=4 ;

	/* PLAYER STATS (size 0x178) */
	zone.stats = pointer;
	/* skip */
	pointer += 0x178;

	/* KILL COUNT (size 0x4c) */
	zone.kills = pointer;
	/* skip */
	pointer += 0x4c;

	/* TAGS (size 0x10) */
	zone.tags = pointer;
	/* skip */
	pointer += 0x10;

	/* PERKS */
	zone.perks = pointer;
	/* skip perks (0x2c8) (4*178) */
	pointer += 0x2c8;

	/* PARTY */
	/* skip party */
	pointer += 4*119*np+0xF0;
	/* FIXME : why the 0xF0 offset ???? */


	/* COMBAT */
	n = read_BE_int(pointer);
	pointer += 4;
	if ( n != 2 ) {
		fprintf(stderr,_FAIL_);
		fprintf(stderr,"Combat mode != 2.\n");
		return zone;
	}

	/* PARTY AI
		blocks of 180 (0xb4) bytes but number of blocks is unknown !

		second int after PARTY AI seems to be always 12 or 10
		third int after PARTY AI seems to be always 10
		as experience SHOULD BE != 10, we can check that on third int

		Hack used : check third int of a block, if it is set to 10, stop.

	*/
	n = 10;
	count = 0;
	while( n == 10){
		pointer += 0xb4;		/* skip a party block */
		n = read_BE_int(pointer+8);	/* read third int ahead */
		count++;
	}
	fprintf(stdout,_INFO_);
	fprintf(stdout,"PARTY_AI : %i blocks.\n",count);
	/* check skill points (4 bytes ahead) */
	n = read_BE_int(pointer+4);
	if ( n > 100) {
		fprintf(stderr,_FAIL_);
		fprintf(stderr,"skill points > 100.\n");
	}

	/* EXPERIENCE */
	zone.exp = pointer;
	/* skip experience */
	pointer += 0x14;

	/* TRAITS */
	zone.traits = pointer;
	/* skip traits */
	pointer += 8;

	return zone;
}
