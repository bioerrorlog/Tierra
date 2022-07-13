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
LLATDOS_CcbEXEbtierradexe =  -c -LC:\BC4\LIB -Tpe -ax
RLATDOS_CcbEXEbtierradexe = 
BLATDOS_CcbEXEbtierradexe = 
CNIEAT_CcbEXEbtierradexe = -IC:\BC4\INCLUDE -D
LNIEAT_CcbEXEbtierradexe = -x
LEAT_CcbEXEbtierradexe = $(LLATDOS_CcbEXEbtierradexe)
REAT_CcbEXEbtierradexe = $(RLATDOS_CcbEXEbtierradexe)
BEAT_CcbEXEbtierradexe = $(BLATDOS_CcbEXEbtierradexe)

#
# Dependency List
#
Dep_tierra = \
   C:\tierra\tierra.exe

tierra : BccDos.cfg $(Dep_tierra)
  echo MakeNode 

Dep_CcbEXEbtierradexe = \
   bookeep.obj\
   decode.obj\
   diskbank.obj\
   frontend.obj\
   genio.obj\
   instruct.obj\
   memalloc.obj\
   memtree.obj\
   portable.obj\
   queues.obj\
   rambank.obj\
   slicers.obj\
   tierra.obj\
   trand.obj\
   tsetup.obj\
   ttools.obj

C:\tierra\tierra.exe : $(Dep_CcbEXEbtierradexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_CcbEXEbtierradexe) $(LNIEAT_CcbEXEbtierradexe) +
C:\BC4\LIB\c0x32.obj+
bookeep.obj+
decode.obj+
diskbank.obj+
frontend.obj+
genio.obj+
instruct.obj+
memalloc.obj+
memtree.obj+
portable.obj+
queues.obj+
rambank.obj+
slicers.obj+
tierra.obj+
trand.obj+
tsetup.obj+
ttools.obj
$<,$*
C:\BC4\LIB\dpmi32.lib+
C:\BC4\LIB\cw32.lib
# C:\BC4\LIB\bgi32.lib+

|

bookeep.obj :  bookeep.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ bookeep.c
|

decode.obj :  decode.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ decode.c
|

diskbank.obj :  diskbank.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ diskbank.c
|

frontend.obj :  frontend.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ frontend.c
|

genio.obj :  genio.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ genio.c
|

instruct.obj :  instruct.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ instruct.c
|

memalloc.obj :  memalloc.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ memalloc.c
|

memtree.obj :  memtree.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ memtree.c
|

portable.obj :  portable.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ portable.c
|

queues.obj :  queues.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ queues.c
|

rambank.obj :  rambank.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ rambank.c
|

slicers.obj :  slicers.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ slicers.c
|

tierra.obj :  tierra.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ tierra.c
|

trand.obj :  trand.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ trand.c
|

tsetup.obj :  tsetup.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ tsetup.c
|

ttools.obj :  ttools.c
  $(BCCDOS) -P- -c @&&|
 $(CEAT_CcbEXEbtierradexe) $(CNIEAT_CcbEXEbtierradexe) -o$@ ttools.c
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-WX
-R
-v
-3
-j50
-g200
-Tpe
| $@


