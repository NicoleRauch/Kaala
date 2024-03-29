# Makefile for kaala.exe
# To compile all necessary files simply type make at the command line prompt.
# This makefile runs with GNU make (version 3.??) only

# If you want to have sound enabled, set HAVEMMLIBS to 1
# Note: Trailing space characters are not allowed here! 
HAVEMMLIBS = 0
# To optimize set OPTIMIZE to 1
OPTIMIZE = 0
# To use method (E2) for compiling as described in emxdev.doc/emxdev.inf
# set USEOMFLIBS to 1
USEOMFLIBS = 0
ifeq ($(USEOMFLIBS),0)
   O = .o						# extension of object files
else
   O = .obj
endif

ifeq ($(OPTIMIZE),0)			# use the following options for debugging 
   CFLAGS = -g -O0 -Wall -Zmtd 
else								# or for optimization:
   CFLAGS = -O2 -fomit-frame-pointer -Wall -Zmtd
endif

# Flags for linking
ifeq ($(USEOMFLIBS),0)
   LDFLAGS = -Zmtd
else
   LDFLAGS = -Zmtd -Zomf
   CFLAGS := $(CFLAGS) -Zomf
endif
ifneq ($(OPTIMIZE),0)		
# The options -s doesn't optimize but strips the symbol table which
# is added to each file. So the code produced with -s isn't faster but the 
# executable is smaller. One the other hand, stripping symbols makes 
# debugging impossible.
   LDFLAGS:= $(LDFLAGS) -s
endif

OBJ = game$O kaala$O pmgame$O #rand$O misc$O errmsgs$O profile$O mmsound$O valuat$O
RES = kaala.res
CC = gcc
PP = cpp -P
RM = rm
MV = move
ifeq ($(HAVEMMLIBS),0)
   LIBS=-los2 -liostream
	EXE=kaala.exe
else
   CFLAGS:= $(CFLAGS) -DUSEMMSOUND
   LIBS=-los2me -los2 -liostream
	EXE=kaalamm.exe
endif

# This is an implicit rule. It causes that any source code that has
# to be compiled from *.cc to *.o is compiled like the following example:
# foo.o: foo.cc foo.h
# 		$(CC) $(CFLAGS) -c foo.cc
%$O:%.cc
	$(CC) $(CFLAGS) -c $<

# The first (default) rule in this makefile. It is taken if make is 
# evoken without any parameter 
all: $(EXE) #kaala.hlp

$(EXE): $(OBJ) kaala.def $(RES)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) kaala.def $(LIBS)
	rc $(RES) $@


# The online help compiler (IPFC) doesn't do any preprocessing. Therefore
# we simply use the c-preprocessor of our compiler. If you want to use
# another preprocessor you have to find out how to invoke him so that
# he doesn't create linenumber informations. This command should then 
# replace the macro definition of $(PP) at the beginning of this makefile.
#kaala.hlp: kaala.i0 defs.h
#	$(PP) $< >kaala.ipf && ipfc /S kaala.ipf


# This target is defined to produce a text only version of the program for 
# testing the game algorithms.
# To make testgame.exe you have to type "make testgame.exe" 
# It's simple, isn't it?
testgame.exe: testgame.cc game$O #rand$O misc$O valuat$O
	$(CC) -DDUMP_DISPLAY -DPRINT_STATS $(CFLAGS) -o testgame.exe $^  $(LIBS)

#valuat$0: valuat.cc valuat.h

#rand$O: rand.cc rand.h

#misc$O: misc.cc misc.h

#profile$O: profile.cc profile.h kaala.h

#errmsgs$O: errmsgs.cc errmsgs.h 

# Do a bit more optimization for the minimax algorithm.
game$O: game.cc game.h
	$(CC) $(CFLAGS) -funroll-loops -c $<

pmgame$O: pmgame.cc pmgame.h game.h defs.h

#mmsound$O: mmsound.cc mmsound.h defs.h

kaala$O: kaala.cc kaala.h game.h defs.h #profile.h

kaala.res: kaala.rc defs.h #help.rc dialog.dlg rhand.ptr yhand.ptr
	rc -r kaala.rc 


# To use the next two targets you need the unix tool rm (removes files)
clean:
	$(RM) -f $(OBJ) $(RES) kaala

veryclean: clean
	$(RM) -f c4nomm.exe c4mm.exe testgame.exe
