
import struct

# FALLOUT 2 savegame mapper

# Settings
#
# number of GVARs
ngvar=791
# party size
np=27

# load objects list
with open("objects.lst") as lst:
	objlist = lst.readlines()
lst.close()

def getzone(save):
	zone={'name':0,'gvars':0,'gvarscopy':0,'inventory':0,'stats':0,'tags':0,'perks':0,'xp':0,'traits':0}
	# check header
	header = struct.unpack("17s",save[0:17])
	if ( header[0] != 'FALLOUT SAVE FILE' ):
		return zone
	# Charname
	zone['name']=0x1d
	# skip header
	offset=0x7567
	# GVARS
	zone['gvars']=offset
	# skip GVARs
	offset+=4*ngvar
	# read number of open maps
	n = struct.unpack(">i",save[offset:offset+4])[0]
	offset+=4
	# skip maps list
	for i in range(n):
		while( save[offset] != '\x00' ) :
			offset+=1
		offset+=1
	# skip after maps field
	offset=offset+4
	# GVARS copy
	zone['gvarscopy']=offset
	# skip GVARs copy
	offset=offset+4*ngvar
	# inventory
	zone['inventory']=offset
	# get number of objects @ 0x48
	count = struct.unpack(">i",save[offset+0x48:offset+0x4C])[0]
	# jump to objects list
	offset=offset+0x80
	# skip object list
	for i in range(count):
		objid = struct.unpack(">23i",save[offset:offset+92])[12]
		offset+=92
		objprop = objlist[objid-1].split(":")
		# print objprop[2].strip("\n")
		objtype = int(objprop[1])
		if objtype > 2 :
			offset+=4
		if objtype == 3 :
			offset+=4
	# skip unknown field
	offset+=4
	# player stats
	zone['stats']=offset
	offset+=0x178
	# kill count (size 0x4c)
	offset+=0x4c
	# tags (size 0x10)
	zone['tags']=offset
	offset+=0x10
	# perks
	zone['perks']=offset
	offset+=0x2c8
	# skip party
	# FIXME : why the 0xF0 ?
	offset+=4*0x77*np+0xF0
	# combat
	# TODO : handle combat mode == 3
	n = struct.unpack(">i",save[offset:offset+4])[0]
	offset+=4
	if ( n != 2 ):
		print "Combat mode != 2"
		return zone
	# skip party ai (seems to be always 22 blocks long)
	n = 10
	count=0
	while( n == 10):
		# skip a party block
		offset+=0xb4
		# read third int ahead
		n = struct.unpack(">i",save[offset+8:offset+12])[0]
		count+=1
	print "Party AI :",count,"blocks"
	# check skill points (4 bytes ahead)
	n = struct.unpack(">i",save[offset+4:offset+8])[0]
	if ( n > 100):
		print "Skill points > 100"
		return zone
	# experience
	zone['xp'] = offset
	offset += 0x14
	# traits
	zone['traits'] = offset
	offset += 8

	return zone


