# the compiler: gcc for C program, define as g++ for C++
CC = gcc

ILIBDIR = ./ilib

ODIR = ./obj

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall -g

# root source file
SRC = main.c

# all .c file in ilib folder
CLIBFILES := $(wildcard $(ILIBDIR)/*.c)

# all .o file compile from ilib folder
CLIBOBJS := $(patsubst $(ILIBDIR)/%.c, $(ODIR)/%.o, $(CLIBFILES))

# header for ilib
ILIBDEPS = ilib.h


# the build target executable:
TARGET = main

# compile all file in ilib
$(ODIR)/%.o: $(ILIBDIR)/%.c  $(ILIBDIR)/$(ILIBDEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

# compile main file and link to ilib
$(TARGET): $(CLIBOBJS)  $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(CLIBOBJ)

# remove all .o file
clean:
	$(RM) $(CLIBOBJS)
