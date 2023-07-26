########################################################################
#                                                                      
#              --- CAEN SpA - Computing Division ---                   
#                                                                      
#   V65xxController Software Project                                        
#                                                                      
#   Created  :  July 2012                                              
#                                                                      
#   Auth: support.computing@caen.it                                                 
#                                                                      
########################################################################

EXE	=	../bin/V6533Controller

CC	=	gcc

COPTS	=	-fPIC -DLINUX -Wall 

FLAGS	=	-Wall -s

DEPLIBS	=       -l CAENComm -lc -lm -lncurses

LIBS	=	

INCLUDEDIR =	-I../include/

OBJS	=	../src/V6533Controller.o ../src/console.o

INCLUDES =	../include/console.h ../include/v6533.h

#########################################################################

all	:	$(EXE)

clean	:
		/bin/rm -f $(OBJS) $(EXE)

$(EXE)	:	$(OBJS)
		/bin/rm -f $(EXE)
		$(CC) $(FLAGS) -o $(EXE) $(OBJS) $(DEPLIBS)

$(OBJS)	:	$(INCLUDES) Makefile

%.o	:	%.c
		$(CC) $(COPTS) $(INCLUDEDIR) -c -o $@ $<

