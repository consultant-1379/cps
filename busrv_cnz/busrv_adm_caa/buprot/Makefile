# **********************************************************************
#
# Short description:
# Makefile for buprot
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
#            2011-01-26   xquydao   Updated for making it build when healder file changes
#            2011-03-17   xquydao   Updated for SDK_APG43L_PROJ_0504_006
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
BUPROT_OBJ = $(OBJDIR)/ALogData.o \
		$(OBJDIR)/APZVersion.o \
		$(OBJDIR)/BackupInfo.o \
		$(OBJDIR)/BUPMsg.o \
		$(OBJDIR)/ExchangeId.o \
		$(OBJDIR)/FtpFileSet.o \
		$(OBJDIR)/DateTime.o \
		$(OBJDIR)/FtpPath.o \
		$(OBJDIR)/Key.o \
		$(OBJDIR)/GAMaintMsg.o \
		$(OBJDIR)/GASrmMsg.o \
		$(OBJDIR)/MiddleWare.o \
		$(OBJDIR)/SectorInfo.o \
		$(OBJDIR)/SegmentHeader.o \
		$(OBJDIR)/APZHwVariant.o \
		$(OBJDIR)/Version.o
                
# Define COMMON paths
COMMON_INCDIR = ../common/inc
#BUP_INCDIR = /vobs/cm4ap/sio/sio_fms/cpf_cnz/cpfapi_caa/inc_ext

# Define own Include paths
CINCLUDES += -I$(INCDIR) -I$(COMMON_INCDIR) -I$(BOOST_SDK_INC) -I$(3PP_SDK_PATH) -I$(COREMW_SDK_INC) -I$(ACE_SDK_INC)

# Define own CFLAGS
ifeq ($(COVERAGE),1)
	CFLAGS += -O0
else
	CFLAGS += -Os
endif
#CFLAGS += -g -Wall -ansi -DNDEBUG -DLOCAL_BUILD
CFLAGS += -g -Wall -ansi -DNDEBUG 

# Define TARGET
TARGET = libbuprot.a

.PHONY: all
all: $(OBJDIR)/$(TARGET)

$(OBJDIR)/$(TARGET): $(BUPROT_OBJ)
	$(SILENT)$(ECHO) 'Creating static library: $(TARGET)'
	$(SILENT)$(AR) -rcs $(OBJDIR)/$(TARGET) $(BUPROT_OBJ)	
	$(NEW_LINE)

# Include dependecies in order to build when header file changes
DEPS = $(BUPROT_OBJ:.o=.d)
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

