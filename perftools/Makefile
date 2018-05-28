# Parallelization:  Sieve of Eratosthenes
# By Aaron Weeden, Shodor Education Foundation, Inc.
# January 2012
#
# Makefile
#
# Usage:  On the command line, enter 'make' followed by one of the options below
# e.g. $ make serial

#------- Select your Compier
COMPTYPE="Cray"
#COMPTYPE="Intel"
#COMPTYPE="GNU"
#COMPTYPE="PGI"

#------ Select known target machine
#SYSTYPE="Gordon"
#SYSTYPE="Stampede"
SYSTYPE="BW"
#SYSTYPE="Mac"
#SYSTYPE="Linux"

ifeq ($(SYSTYPE), "Gordon")
CC			= icc
MPICC		= mpicc
OMPFLAGS	= -openmp
LIBS		= -lm
endif

ifeq ($(SYSTYPE), "BW")
    ifeq ($(COMPTYPE), "Cray")
    CC			= cc
    MPICC		= cc
    CFLAGS      = -h profile_generate
    FFLAGS      = -h profile_generate
    LIBS		= -lm
    else ifeq ($(COMPTYPE), "GNU")
    CC          = gcc
    MPICC       = cc
    OMPFLAGS    = -fopenmp
    LIBS        = -lm
    endif
endif

ifeq ($(SYSTYPE), "Stampede")
CC          = icc
MPICC       = mpicc
OMPFLAGS    = -openmp
LIBS        = -lm
endif

matrixmult:
	$(CC) -o matrixmult.exe matrixmult.c -lm
mm_gprof:
	gcc -g -o mm_grpof.exe matrixmult.c -pg -lm
mm_craypath:
	$(CC) -h profile_generate -o mm_craypath.exe matrixmult.c -lm
mm_reveal:
	$(CC) -O3 -h pl=mm_reveal.exe.pl -h wp -o mm_craypath.exe matrixmult.c -lm
all:
	make clean
	make matrixmult mm_grpof.exe mm_craypath.exe 
clean:
	rm -rf *.exe
