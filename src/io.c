#include "io.h"

/*

TODO :

objsubtype should read pro_filename from items.lst

*/


/* string copy, yes */
void strcopy( char *a, char *b ){
	int i=0;
	while ( a[i] != 0 ){
		b[i] = a [i];
		i++;
	}
	b[i] = 0;
	return;
}

/* set proid filename from integer */
void setproid( char *name , int n ){
	int i=0,r;
	/* name -> "proto/items/12345678.pro" */
	name+=20;
	/* name -> ".pro" */
	/* set backward */
	for(i=0;i<8;i++){
		name--;
		r=n%10;
		name[0]=r+0x30; /* convert to ascii */
		n/=10;
	}
	return;
}

/* return object type */
int objsubtype ( int n ){

	char		defname[]="proto/items/12345678.pro";
	char		proname[24];
	unsigned char	*buffer;
	int		size;
	int		type;

	if ( n == 0) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"(objsubtype) null object query.\n");
		return -1;
	}

	/* FIXME : HARDCODED WORKAROUND
		items.lst differs from logical order
		objid -> line number in items.lst -> pro_file -> subtype
	*/
	switch(n){
		case  1: n= 3; break;
		case  2: n= 1; break;
		case  3: n=14; break;
		case  4: n=12; break;
		case  5: n= 9; break;
		case  6: n= 5; break;
		case  7: n=13; break;
		case  8: n= 4; break;
		case  9: n= 2; break;
		case 10: n=11; break;
		case 11: n=10; break;
		case 12: n= 6; break;
		case 13: n= 7; break;
		case 14: n= 8; break;
	}

	strcopy(defname,proname);
	setproid(proname,n);
	size=loadfromdat2(&buffer,proname,"MASTER.DAT");

	if ( size == 0)	return -1;

	/* goto subtype */
	buffer+=0x20;
	type = read_BE_int(buffer);
/*
	0 - Armor
	1 - Container
	2 - Drug
	3 - Weapon
	4 - Ammo
	5 - Misc
	6 - Key
*/
	if ( type > 6) type = -1;

	/* go back to start and free */
	buffer-=0x20;
	free(buffer);

	return type;
}

/* compare to blocks of data */
int sameblock( unsigned char *a, unsigned char *b, int n){
	int i;
	for(i=0;i<n;i++){
		if (a[i] != b[i]) return 0;
	}
	return 1;
}

/* same string, yes */
int samestring( char *a, char *b){
	int i=0;
	while (a[i] != 0) {
		if (a[i] != b[i]) return 0;
		i++;
	}
	if (a[i] != b[i]) return 0;
	return 1;
}

/* read big endian integer (4 bytes, 32 bits) */
int read_BE_int( unsigned char *pointer) {
	long int	k=0;
	int		i;
	for (i=0;i<4;i++) { k<<=8; k+=pointer[i]; }
	return k;
}

/* write big endian integer (4 bytes, 32 bits) */
void write_BE_int( int k, unsigned char *out ) {
	unsigned char	*in;
	in = (unsigned char *) &k;
	out[0]=in[3];
	out[1]=in[2];
	out[2]=in[1];
	out[3]=in[0];
	return;
}

/* inflate zlib stream from "packed" to "real" (needs malloc first !) */
int inflateblock(unsigned char *packed, int psize, unsigned char *real, int rsize)
{
	int ret;
	unsigned have;
	z_stream strm;

	/* init stream */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK) return ret;

	/* decompress until stream ends or end of file */
	do {
		strm.avail_in = CHUNK;
		if ( psize < CHUNK ) { strm.avail_in = psize; }

	        if (strm.avail_in == 0) break;

	        strm.next_in = packed;
		packed+=strm.avail_in;	/* packed -> next chunk */
		psize-=strm.avail_in;	/* psize = bytes left */

		do {
			strm.avail_out = CHUNK;
			if ( rsize < CHUNK ) { strm.avail_out = rsize; }
			strm.next_out = real;
			ret = inflate(&strm, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret) {
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;     /* and fall through */
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
				return ret;
			}
			have = CHUNK - strm.avail_out;
			real+= have;
			rsize-=have;
	        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

/* normalize a string */
void lowercase( char *str )
{
	int i=0;
	while(str[i] != 0){
		/*  "A"=0x41 "Z"=0x5A -> "a"=0x61 "z"=0x7A */
		if ( str[i]>0x40 && str[i]<0x5B ) { str[i] += 0x20; }
		/* unix slashes */
		if (str[i]=='\\') { str[i] = '/'; }
		i++;
	}
	return;
}

/* normalize a string over n char */
void lowercaseover( char *str, int n )
{
	int i;
	for(i=0;i<n;i++){
		/*  "A"=0x41 "Z"=0x5A -> "a"=0x61 "z"=0x7A */
		if ( str[i]>0x40 && str[i]<0x5B ) { str[i] += 0x20; }
		/* unix slashes */
		if (str[i]=='\\') { str[i] = '/'; }
	}
	return;
}

/* fread helper (malloc included) */
int loadfromfile(unsigned char **pointer, char *filename){
	FILE	*stream;
	int	size;

	stream = fopen(filename,"rb");
	if ( stream == NULL ) {
		fprintf(stdout,_FAIL_);
		fprintf(stderr,"(loadfromfile) can't open %s.\n",filename);
		return -1;
	}
	fseek(stream,0,SEEK_END);
	size=ftell(stream);
	fseek(stream,0,SEEK_SET);
	*pointer=malloc(size);
	if ( *pointer == NULL ) {
		fprintf(stdout,_FAIL_);
		fprintf(stderr,"(loadfromfile) can't allocate %i bytes.\n",size);
		return -1;
	}
	fread(*pointer,1,size,stream);
	fclose(stream);

	return size;
}

void getmsg( char * str, unsigned char *msg , int n){

	int		id=0;
	int		count;

	while( id != n) {
		id=0;
		/* jump to "{" */
		while( msg[0] != 0x7b ) msg++;
		/* skip "{" */
		msg++;
		/* read ASCII coded int */
		while( msg[0] != 0x7d ) {
			id*=10;
			id+=(unsigned int) msg[0] - 0x30;
			msg++;
		}
		/* skip "}" */
		msg++;
		/* skip "{}" */
		msg+=2;
		/* if not "{", EOF, return empty string */
		if ( msg[0] != 0x7b ) { str[0]=0; return; }
		/* skip "{" */
		msg++;
		/* copy until "}" */
		count=0;
		while( msg[0] != 0x7d ) {
			str[0] = msg[0];
			msg++;
			str++;
			count++;
		}
		str[0]=0;
		str-=count;
	}	
	return;
}

/* load file from DAT (malloc included) */
int loadfromdat2(unsigned char **pointer, char *filename, char *datname ) {

	FILE 			*datfile;
	int			n,i;
	int			psize,rsize,size,treesize,datasize;
	unsigned char		compressed;
	int			offset;
	int			count;
	char			name[256];

	unsigned char		*buffer_in;
	unsigned char		*buffer_out;

	/* WARNING : do NOT work with constant ! */
	lowercase(filename);

	datfile = fopen(datname,"rb");
	if ( datfile == NULL ) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"(loadfromdat2) can't open %s.\n",datname);
		return 0;
	}
	fseek(datfile,0,SEEK_END);
	size=ftell(datfile);

	/* jump to end minus 8 bytes */
	fseek(datfile,-8,SEEK_END);
	/* read tree size */	
	fread(&treesize,4,1,datfile);
	/* read data size */
	fread(&datasize,4,1,datfile);

	if( datasize != size) {
		fprintf(stdout,_FAIL_);
		fprintf(stderr,"(loadfromdat2) size mismatch, %s is not a Fallout 2 DAT.\n",datname);
		fclose(datfile);
		return 0;
	}

	/* jump to tree */
	fseek(datfile,datasize-treesize-8,SEEK_SET);
	/* read count */
	fread(&count,4,1,datfile);

	while( samestring(name,filename) == 0 ) {
		/* zeroing name */
		for(i=0;i<256;i++){ name[i]=0; }
		/* read block */
		fread(&n,4,1,datfile);
		fread(&name,1,n,datfile);
		lowercaseover(name,n);
		fread(&compressed,1,1,datfile);
		fread(&rsize,4,1,datfile);
		fread(&psize,4,1,datfile);
		fread(&offset,4,1,datfile);
		count--;
		if( count == 0) {
			fprintf(stdout,_FAIL_);
			fprintf(stdout,"(loadfromdat2) can't find %s in %s.\n",filename,datname);
			break;
		}
	}

	if( count == 0) return 0;

	/* infos */
/*
	fprintf(stdout,"File        : %s\n",name);
	fprintf(stdout,"Status      : ");
	if ( compressed == 0 ) { fprintf(stdout,"plain\n"); }
	if ( compressed == 1 ) { fprintf(stdout,"compressed\n"); }
	if ( compressed > 1 ) { fprintf(stdout,"%i\n",compressed); }
	fprintf(stdout,"Packed size : %i\n",psize);
	fprintf(stdout,"Real size   : %i\n",rsize);
*/
	buffer_in=malloc(psize);
	if ( buffer_in == NULL ) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"(loadfromdat2) can't allocate buffer_in (%i bytes).\n",rsize);
		fclose(datfile);
		return 0;
	}

	/* load block */

	/* jump to offset */
	fseek(datfile,offset,SEEK_SET);
	/* read psize bytes */
	fread(buffer_in,1,psize,datfile);
	/* close file stream */
	fclose(datfile);

	if ( compressed == 0 ) {
		*pointer = buffer_in;
		return rsize;
	}

	buffer_out=malloc(rsize);
	if ( buffer_out == NULL ) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"(loadfromdat2) can't allocate buffer_out (%i bytes).\n",rsize);
		free(buffer_in);
		return 0;
	}

	/* inflate */
	if ( inflateblock(buffer_in, psize, buffer_out, rsize) != Z_OK ) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"(loadfromdat2) error while inflating.\n");
		free(buffer_in);
		free(buffer_out);
		return 0;
	}
	free(buffer_in);

	*pointer = buffer_out;
	return rsize;

}

/* fwrite helper */
int savetofile(unsigned char *pointer, int size, char *filename){
	FILE	*stream;
	int	n;

	stream = fopen(filename,"wb");
	if ( stream == NULL ) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"(savetofile) can't open %s.\n",filename);
		return -1;
	}
	n=fwrite(pointer,1,size,stream);
	fclose(stream);
	fprintf(stdout,_INFO_);
	fprintf(stdout,"%i bytes saved to %s.\n",n,filename);
	return n;
}

/* helper */
int isf2sav( char *filename ) {

	FILE 			*savefile;
	char			buffer[256];
	char			header[]="FALLOUT SAVE FILE";

	savefile = fopen(filename,"rb");
	if ( savefile == NULL ) {
		fprintf(stdout,_FAIL_);
		fprintf(stdout,"(isf2sav) can't open %s.\n",filename);
		return 0;
	} 

	fread(buffer,1,0x18,savefile);
	fclose(savefile);

	return samestring( buffer, header );

}
