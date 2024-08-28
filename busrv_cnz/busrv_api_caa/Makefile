# **********************************************************************
#
# Short description:
# Makefile for BUSRV library
# **********************************************************************
#
# Ericsson AB 2013 All rights reserved.
# The information in this document is the property of Ericsson.
# Except as specifically authorized in writing by Ericsson, the receiver of this
# document shall keep the information contained herein confidential and shall protect
# the same in whole or in part from disclosure and dissemination to third parties.
# Disclosure and disseminations to the receivers employees shall only be made
# on a strict need to know basis.
#
# **********************************************************************
#
# Rev        Date         Name      What
# -----      ---------    --------  --------------------------
#            2012-xx-xx   xdtthng   Created
#            2013-03-01   xngudan   Updated
#
# ********************************************************************

# **********************************************************************
#
#
# ********************************************************************

#the following variables are defined in common.mk file
#BLOCKDIR = $(CURDIR)
#OBJDIR   = $(BLOCKDIR)/obj
#SRCDIR   = $(BLOCKDIR)/src
#INCDIR   = $(BLOCKDIR)/inc
#TESTDIR  = $(BLOCKDIR)/test
#OUTDIR   = $(CXC_PATH)/bin

# BEGIN : GIT Intro

CNZ_NAME ?= busrv_cnz
CAA_NAME ?= busrv_api_caa
CXC_NAME ?= busrv_cxc

# Basic paths
VOB_PATH = $(CPS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

CXC_PATH = ../busrv_cxc

# Define lib variables
LIB_NAME = cps_sbc
LIB_REL_MAJOR ?= 1
LIB_REL_MINOR ?= 1
LIB_REL_BUILD ?= 0
LIB_VER ?= $(LIB_REL_MAJOR).$(LIB_REL_MINOR).$(LIB_REL_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VER)
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_REL_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

BUCP_BUFILE_APLIB = $(OUTDIR)/$(LIB_FILENAME)
BUCP_BUFILE_APLIB_SO = $(OUTDIR)/$(LIB_LINKER_NAME)

# Define COMMON & BUPORT paths
COMMON_DIR     = ../busrv_adm_caa/common
COMMON_INCDIR  = $(COMMON_DIR)/inc
COMMON_OBJDIR  = $(COMMON_DIR)/obj
COMMON_LIB     = $(COMMON_OBJDIR)/libcommon.a
COMMON_SRCDIR = $(COMMON_DIR)/src
COMMON_OBJS = $(COMMON_OBJDIR)/GAClient.o

BUPROT_DIR     = ../busrv_adm_caa/buprot
BUPROT_INCDIR  = $(BUPROT_DIR)/inc
BUPROT_OBJDIR  = $(BUPROT_DIR)/obj
BUPROT_LIB     = $(BUPROT_OBJDIR)/libbuprot.a
BUPORT_SRCDIR = $(BUPROT_DIR)/src
BUPROT_OBJS = $(BUPROT_OBJDIR)/GASrmMsg.o

BUSRV_DIR     	= ./
BUSRV_INCDIR	= $(BUSRV_DIR)/inc_ext


# If modified, must be placed after the above line
OUTDIR   = $(CXC_PATH)/bin/lib_ext

# Define objs want to build
BUPDEF_OBJ = $(OBJDIR)/CPS_BUSRV_API_Sbc_R1.o \
	$(OBJDIR)/CPS_BUSRV_API_Sbc_Impl.o
				      
# Define own Include paths
CINCLUDES += -I$(INCDIR) -I$(BUPROT_INCDIR) -I$(COMMON_INCDIR) -I$(BUSRV_INCDIR) -I$(BOOST_SDK_INC) -I$(AP_SDK_INC)

# Define own lib paths
LIBSDIR += -L$(BOOST_SDK_LIB) -L$(AP_SDK_LIB) -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB)  -L$(SEC_CREDU_SDK_LIB)

# Define own Libs 
LIBS += -lacs_dsd

# Define own CFLAGS
ifeq ($(COVERAGE),1)
	CFLAGS += -O0
else
	CFLAGS += -Os
endif
#CFLAGS += -g -Wall -ansi -DNDEBUG -DLOCAL_BUILD
CFLAGS += -g -Wall -ansi -DNDEBUG -std=c++0x
LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME)

.PHONY: all

all: $(OUTDIR)/$(LIB_FILENAME)

$(OUTDIR)/$(LIB_FILENAME): $(BUPDEF_OBJ) buprot common
	$(SILENT)$(ECHO) 'Creating library: $(OUTDIR)/$(LIB_FILENAME)'
	$(SILENT)$(CC) -o $(BUCP_BUFILE_APLIB) $(BUPDEF_OBJ) $(COMMON_OBJS) $(BUPROT_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(LIB_FILENAME))
	$(SILENT)$(ECHO) '$(OUTDIR)/$(LIB_FILENAME) is created'
	$(NEW_LINE)	

# Include dependecies in order to build when header file changes
DEPS = $(BUPDEF_OBJ:.o=.d)
ifeq ($(MAKECMDGOALS),all)	
	-include $(DEPS)
else
	ifeq ($(MAKECMDGOALS),)
		-include $(DEPS)
	endif
endif

.PHONY: buprot
buprot:
# These command will be executed in another shell (not the current shell)
	cd $(BUPROT_DIR); $(MAKE) $(MFLAGS) all

.PHONY: common
common:
# These command will be executed in another shell (not the current shell)
	cd $(COMMON_DIR); $(MAKE) $(MFLAGS) all

# this rule is defined in common.mk
#$(OBJDIR)/%.o: $(SRCDIR)/%.cpp

.PHONY: clean
clean:
	cd $(BUPROT_DIR); $(MAKE) $(MFLAGS) clean;
	cd $(COMMON_DIR); $(MAKE) $(MFLAGS) clean;
	$(SILENT)$(RM) -r $(OBJDIR)/*

.PHONY: distclean
distclean: clean
	$(SILENT)$(RM) -r $(OUTDIR)/$(LIB_FILENAME)
	$(SILENT)$(RM) -r $(DEBUGDIR)/$(LIB_FILENAME)_dbg

