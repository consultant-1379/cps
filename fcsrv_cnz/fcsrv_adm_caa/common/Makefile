# **********************************************************************
#
# Short description:
# Makefile for common
# **********************************************************************
#
# Ericsson AB 2010 All rights reserved.
# The information in this document is the property of Ericsson.
# Except as specifically authorized in writing by Ericsson, the receiver of this
# document shall keep the information contained herein confidential and shall protect
# the same in whole or in part from disclosure and dissemination to third parties.
# Disclosure and disseminations to the receivers employees shall only be made
# on a strict need to know basis.
#
# **********************************************************************
#
# Rev		 Date		  Name      What
# -----      ---------    --------  --------------------------
#		     2010-11-09   xquydao   Created
#            2011-01-06   xquydao   Updated for SDK_APG43L_PROJ_0504_001
#            2011-01-25   xquydao   Updated for making it build when healder file changes
#            2011-03-17   xquydao   Updated for SDK_APG43L_PROJ_0504_006
#            2011-09-15   xdtthng   Moving elements to work with FMS
#            2015-01-25   egiaava   Adapted for SLES12 and new BOOST LIB 1_54
#
# ********************************************************************

# The following variables are defined in common.mk file
#SRCDIR = src
#OBJDIR = obj
#OUTDIR = $(CXCDIR)/bin
#INCDIR = inc

CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk

# Define objs want to build
COMMON_OBJ = $(OBJDIR)/fcap.o \
		$(OBJDIR)/FCPFunx.o \
		$(OBJDIR)/CodeException.o \
		$(OBJDIR)/Config.o \
		$(OBJDIR)/CPS_FCSRV_Trace.o \
		$(OBJDIR)/CPS_Parameters.o \
		$(OBJDIR)/CPS_Utils.o \
		$(OBJDIR)/DateTime.o \
		$(OBJDIR)/ErrorEvent.o \
		$(OBJDIR)/Exception.o \
		$(OBJDIR)/FileFunx.o \
		$(OBJDIR)/LinuxException.o \
		$(OBJDIR)/SystemInfo.o \
		$(OBJDIR)/TCPAcceptor.o \
		$(OBJDIR)/TCPConnection.o \
		$(OBJDIR)/TCPServer.o \
		$(OBJDIR)/TmpCurrDir.o

# Define FCPROT path
FCPROT_INCDIR = ../fcprot/inc
FCSRV_INCDIR = ../fcsrv/inc
#FCP_INCDIR = /vobs/cm4ap/sio/sio_fms/cpf_cnz/cpfapi_caa/inc_ext

# Define own Include paths
CINCLUDES += -I$(INCDIR) -I$(FCPROT_INCDIR) -I$(FCSRV_INCDIR) -I$(BOOST_SDK_INC) -I$(AP_SDK_INC)\
-I$(COREMW_SDK_INC) -I$(ACE_SDK_INC)

# Define own CFLAGS
ifeq ($(COVERAGE),1)
	CFLAGS += -O0
else
	CFLAGS += -Os
endif
#CFLAGS += -g -Wall -ansi -DNDEBUG -DLOCAL_BUILD
CFLAGS += -g -Wall -ansi -DNDEBUG
 
# TODO: do 'make -DHAVE_CONSOLE_TRA' if need to output to CONSOLE 
#CFLAGS += -DHAVE_CONSOLE_TRA

# Define TARGET
TARGET = libcommon.a

.PHONY: all
all: $(OBJDIR)/$(TARGET)

$(OBJDIR)/$(TARGET): $(COMMON_OBJ)
	$(SILENT)$(ECHO) 'Creating static library: $(TARGET)'
	$(SILENT)$(AR) -rcs $(OBJDIR)/$(TARGET) $(COMMON_OBJ)	
	$(NEW_LINE)

# Include dependecies in order to build when header file changes
DEPS = $(COMMON_OBJ:.o=.d)
ifeq ($(MAKECMDGOALS),all)	
	-include $(DEPS)
else
	ifeq ($(MAKECMDGOALS),)
		-include $(DEPS)
	endif
endif
    
# this rule is defined in common.mk
#$(OBJDIR)/%.o: $(SRCDIR)/%.cpp

.PHONY: clean
clean:
	$(SILENT)$(RM) -r $(OBJDIR)/*

