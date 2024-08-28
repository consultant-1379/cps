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
COMMON_OBJ = $(OBJDIR)/APBackupInfo.o \
		$(OBJDIR)/buap.o \
		$(OBJDIR)/BUPFunx.o \
		$(OBJDIR)/BUParamsCommon.o \
		$(OBJDIR)/BUParamsOM.o \
		$(OBJDIR)/CodeException.o \
		$(OBJDIR)/Config.o \
		$(OBJDIR)/CPS_BUSRV_Trace.o \
		$(OBJDIR)/CPS_Parameters.o \
		$(OBJDIR)/CPS_Utils.o \
		$(OBJDIR)/DataFile.o \
		$(OBJDIR)/DSDServer.o \
		$(OBJDIR)/ErrorEvent.o \
		$(OBJDIR)/Exception.o \
		$(OBJDIR)/EventReporter.o \
		$(OBJDIR)/EnvFMS.o \
		$(OBJDIR)/FallbackMgr.o \
		$(OBJDIR)/FileFunx.o \
		$(OBJDIR)/GAClient.o \
		$(OBJDIR)/GAMaintSession.o \
		$(OBJDIR)/GAServer.o \
		$(OBJDIR)/GASrmSession.o \
		$(OBJDIR)/InvalidValueError.o \
		$(OBJDIR)/LinuxException.o \
		$(OBJDIR)/ParseIni.o \
		$(OBJDIR)/SBCFile.o \
		$(OBJDIR)/SBCId.o \
		$(OBJDIR)/SBCList.o \
		$(OBJDIR)/SystemInfo.o \
		$(OBJDIR)/TCPAcceptor.o \
		$(OBJDIR)/TCPConnection.o \
		$(OBJDIR)/TCPServer.o \
		$(OBJDIR)/TmpCurrDir.o

# Define BUPROT path
BUPROT_INCDIR = ../buprot/inc
BUPROT_OBJDIR = ../buprot/obj
BUSRV_INCDIR = ../busrv/inc
BUPROT_OBJ = $(BUPROT_OBJDIR)/GAMaintMsg.o

# Define own Include paths
#CINCLUDES += -I inc -I /vobs/cm4ap/ntacs/apgcc_cnz/apgccapi_caa/OMHandling/inc_ext -I /vobs/cm4ap/ntacs/apgcc_cnz/apgccapi_caa/OMHandling/inc_int -I /vobs/cm4ap/ntacs/apgcc_cnz/apgccapi_caa/ha/app_mngr/inc
CINCLUDES += -I$(INCDIR) -I$(BUPROT_INCDIR) -I$(BUSRV_INCDIR) -I$(BOOST_SDK_INC) -I$(AP_SDK_INC) -I$(3PP_SDK_PATH) -I$(COREMW_SDK_INC) -I$(ACE_SDK_INC)

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
	$(SILENT)$(AR) -rcs $(OBJDIR)/$(TARGET) $(COMMON_OBJ) $(BUPROT_OBJ)
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

