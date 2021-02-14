#!/usr/bin/python

import sys
import struct
import tkFileDialog

from Tkinter import *
from ttk import *

from helpers import *

traitname=('Fast Metabolism','Bruiser','Small Frame','One Hander','Finesse','Kamikaze','Heavy Handed','Fast Shot','Bloody Mess','Jinxed','Good Natured','Chem Reliant','Chem Resistant','Sex Appeal','Skilled','Gifted','[UNUSED]')


filename=""


# GUI stuff
speciallabel=['ST','PE','EN','CH','IN','AG','LK']
svar=[0,0,0,0,0,0,0]

root = Tk()

root.title("Editor")
statusfilename=StringVar()
charname=StringVar()
trait1var=StringVar()
trait2var=StringVar()

def save(*args):
	offset=0
	filename=statusfilename.get()
	if ( filename == '' ):
		return
	# get SPECIAL
	special=[0,0,0,0,0,0,0]
	for i in range(7):
		special[i]=int(svar[i].get())
	# get traits
	trait=[0,0]
	trait[0]=traitname.index(trait1var.get())
	if ( trait[0] == 16 ):
		trait[0] = -1
	trait[1]=traitname.index(trait2var.get())
	if ( trait[1] == 16 ):
		trait[1] = -1
	# read
	fsave = open(filename,"rb")
	save=fsave.read()
	fsave.close()
	# get offsets
	zone=getzone(save)
	# modify stats
	offset=zone['stats']
	if ( offset == 0):
		return
	offset+=8
	save=list(save)
	record=list(struct.pack(">7i",special[0],special[1],special[2],special[3],special[4],special[5],special[6]))
	for i in range(28):
		save[offset+i]=record[i]
	save=''.join(save)
	# modify traits
	offset=zone['traits']
	if ( offset == 0):
		return
	save=list(save)
	record=list(struct.pack(">2i",trait[0],trait[1]))
	for i in range(8):
		save[offset+i]=record[i]
	save=''.join(save)
	# write
	fsave = open(filename,"wb")
	fsave.write(save)
	fsave.close()
	return

def load(*args):
	filename = tkFileDialog.askopenfilename()
	statusfilename.set(filename)
	# read
	fsave = open(filename,"rb")
	save=fsave.read()
	fsave.close()
	# get offsets
	zone=getzone(save)
	# stats
	offset=zone['stats']
	if (offset == 0):
		return
	offset+=8
	special = list(struct.unpack(">7i",save[offset:offset+28]))
	# traits
	offset=zone['traits']
	if (offset == 0):
		return
	trait = list(struct.unpack(">2i",save[offset:offset+8]))
	# update GUI
	for i in range(7):
		svar[i].set(value=special[i])
	trait1var.set(value=traitname[trait[0]])
	trait2var.set(value=traitname[trait[1]])

	return


# GUI
# TODO show charname

content = Frame(root,padding=(10,10,10,10))
box = Frame(root,padding=(10,10,10,10))

for i in range(7):
	l = Label(content,text=speciallabel[i],width=7)
	l.grid(column=0, row=i+0, sticky=(N,W,E,S))
	svar[i]=StringVar(value=0)
	s = Spinbox(content, from_=1.0, to=10.0, textvariable=svar[i])
	s.grid(column=1, row=i+0, sticky=(N,W,E,S))

l = Label(content, text='Trait')
l.grid(column=0, row=7, sticky=(N,W,E,S), columnspan=2)
trait1box = Combobox(content, textvariable=trait1var, state="readonly", values=traitname)
trait1box.grid(column=1,row=7)
l = Label(content, text='Trait')
l.grid(column=0, row=8, sticky=(N,W,E,S), columnspan=2)
trait2box = Combobox(content, textvariable=trait2var, state="readonly", values=traitname)
trait2box.grid(column=1,row=8)

load=Button(box, text="Load", command=load)
save=Button(box, text="Save", command=save)
load.pack()
save.pack()

content.grid(column=0, row=0, sticky=(N, S, E, W))
box.grid(column=0, row=1, sticky=(N, S, E, W))

root.mainloop()



