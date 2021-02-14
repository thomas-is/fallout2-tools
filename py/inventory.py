#!/usr/bin/python

import sys
import struct
import tkFileDialog

from Tkinter import *
from ttk import *

from helpers import *

root = Tk()

root.title("Editor")
svar=[]
statusfilename=StringVar()
filename = tkFileDialog.askopenfilename()
statusfilename.set(filename)
# read
fsave = open(filename,"rb")
save=fsave.read()
fsave.close()
# get offsets
zone=getzone(save)

offset=zone['inventory']

count = struct.unpack(">i",save[offset+0x48:offset+0x4C])[0]

# jump to objects list
offset=offset+0x80
# skip object list
for i in range(count):
	obj=list(struct.unpack(">23i",save[offset:offset+92]))
	offset+=92
	objid = obj[12]
	objprop = objlist[objid-1].split(":")
	objtype = int(objprop[1])

	svar.append(StringVar(value=obj[0]))
	s = Spinbox(root,textvariable=svar[i],width=10)
	s.grid(column=0,row=i)
	l=Label(root,text=objprop[2].strip("\n"))
	l.grid(column=1,row=i)
"""
	l=Label(root,text=obj[17])
	l.grid(column=3,row=i)
	l=Label(root,text=obj[9])
	l.grid(column=4,row=i)

	if objtype > 2 :
		obj.append(struct.unpack(">i",save[offset:offset+4])[0])
		offset+=4
		l=Label(root,text=obj[23])
		l.grid(column=5,row=i)
	if objtype == 3 :
		obj.append(struct.unpack(">i",save[offset:offset+4])[0])
		offset+=4
		ammo=objlist[obj[24]-1].split(":")
		l=Label(root,text=ammo[2].strip("\n"))
		l.grid(column=6,row=i)
"""


root.mainloop()
