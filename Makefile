# Makefile for srec2bin
# (C)2017 Raphael Kim / rageworx@gmail.com
#

#########################################################################
# About cross compiler, or other platform :
#
# To enable build for embedded linux, you may encomment next 2 lines.
# Or, may need to change your cross compiler path with environments.
# It is up to developer !

# CCPREPATH = ${ARM_LINUX_GCC_PATH}
# CCPREFIX  = arm-linux-

# To enable build for embedded linux, change following line.
# CCPATH    = ${CCPREPATH}/${CCPREFIX}
CCPATH =
#########################################################################

# Compiler configure.
GCC = ${CCPATH}gcc
GPP = ${CCPATH}g++
AR  = ${CCPATH}ar

# Sources and how it built
# Optimization issue: recommend to build with using -ffast-math option.
# Change if your CPU architecture supports more high technologies.
INCDIR    = ./src
SOURCEDIR = ./src
OBJDIR    = ./obj/Release
OUTBIN    = srec2bin
OUTDIR    = ./out
DEFINEOPT =
OPTIMIZEOPT = -O3 -s
CPUARCHOPT =
CFLAGS    = -I$(INCDIR) -I$(SOURCEDIR) -I$(FLTKDIR) $(DEFINEOPT) $(OPTIMIZEOPT) $(CPUARCHOPT) $(BITSOPT) $(FLTKLIBS)

all: prepare clean ${OUTDIR}/${OUTBIN}

prepare:
	@mkdir -p ${OBJDIR}
	@mkdir -p ${OUTDIR}

${OBJDIR}/stools.o:
	@$(GPP) -c ${SOURCEDIR}/stools.cpp ${CFLAGS} -o $@

${OBJDIR}/srec.o:
	@$(GPP) -c ${SOURCEDIR}/srec.cpp ${CFLAGS} -o $@


${OUTDIR}/${OUTBIN}: ${OBJDIR}/stools.o ${OBJDIR}/srec.o
	@echo "Generating $@ ..."
	@$(GPP) ${SOURCEDIR}/main.cpp ${CFLAGS} ${OBJDIR}/*.o -o $@

clean:
	@echo "Cleaning built directories ..."
	@rm -rf ${OBJDIR}/*
	@rm -rf ${OUTDIR}/${OUTBIN}*
