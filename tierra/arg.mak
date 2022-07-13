/* @(#)arg.mak	1.1 11/15/94*/
#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCCDOS  = Bcc32 +BccDos.cfg 
TLINK   = TLink32
TLIB    = TLib
TASM    = Tasm
#
# IDE macros
#


#
# Options
#
IDE_LFLAGSDOS =  -LC:\BC4\LIB
IDE_BFLAGS = 
LLATDOS_CcbEXEbargdexe =  -c -LC:\BC4\LIB -Tpe -ax
RLATDOS_CcbEXEbargdexe = 
BLATDOS_CcbEXEbargdexe = 
CNIEAT_CcbEXEbargdexe = -IC:\BC4\INCLUDE -D
LNIEAT_CcbEXEbargdexe = -x
LEAT_CcbEXEbargdexe = $(LLATDOS_CcbEXEbargdexe)
REAT_CcbEXEbargdexe = $(RLATDOS_CcbEXEbargdexe)
BEAT_CcbEXEbargdexe = $(BLATDOS_CcbEXEbargdexe)

#
# Dependency List
#
Dep_arg = \
   C:\tierra\arg.exe

arg : BccDos.cfg $(Dep_arg)
  echo MakeNode 

Dep_CcbEXEbargdexe = \
   arg.obj\
   arg_inc.obj

C:\tierra\arg.exe : $(Dep_CcbEXEbargdexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_CcbEXEbargdexe) $(LNIEAT_CcbEXEbargdexe) +
C:\BC4\LIB\c0x32.obj+
arg.obj+
arg_inc.obj
$<,$*
C:\BC4\LIB\dpmi32.lib+
C:\BC4\LIB\cw32.lib

|

arg.obj :  arg.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbargdexe) $(CNIEAT_CcbEXEbargdexe) -o$@ arg.c
|

arg_inc.obj :  arg_inc.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbargdexe) $(CNIEAT_CcbEXEbargdexe) -o$@ arg_inc.c
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-WX
-R
-v
-vi
-H
-H=arg.csm
-R-
-3
-j50
-g200
-i32
-O
-Og
-Oe
-Om
-Ov
-Ol
-Ob
-Op
-Oi
-Z
-k-
-vi-
-H-
-Tpe
| $@


