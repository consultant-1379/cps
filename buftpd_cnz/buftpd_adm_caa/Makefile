# **********************************************************************
#
# Short description:
# Makefile for buftpd
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
# Rev        Date         Name      What
# -----      ---------    --------  --------------------------
#            2011-07-20   xquydao   Created
#            2012-10-01   xvunguy   Append tha path to log4cplus's header
#
# **********************************************************************

# The following variables are defined in common.mk file
#SRCDIR = src
#OBJDIR = obj
#OUTDIR = $(CXCDIR)/bin
#INCDIR = inc

# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common

CXCDIR = ../buftpd_cxc

BUAP_API_INC = ../../buap_cnz/buap_api_caa/inc
BUAP_API_LIB = ../../buap_cnz/buap_cxc/bin

BUFTPD_APNAME   = cps_buftpd
BUFTPD_APEXE    = $(OUTDIR)/$(BUFTPD_APNAME)

CNZ_NAME ?= buftpd__cnz
CAA_NAME ?= buftpd_adm_caa
CXC_NAME ?= buftpd__cxc

# Basic paths
VOB_PATH = $(CPS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)


include $(COMMON_ROOT)/common.mk

# Define objs want to build
BUFTPD_OBJ = $(OBJDIR)/CPS_FTP_Connection.o \
			$(OBJDIR)/CPS_FTP_EventReporter.o \
			$(OBJDIR)/CPS_FTP_Events.o \
			$(OBJDIR)/CPS_FTP_Main.o \
			$(OBJDIR)/CPS_FTP_Server.o \
			$(OBJDIR)/CPS_FTP_Service.o \
			$(OBJDIR)/CPS_FTP_Trace.o

# Define own Include paths
CINCLUDES += -I$(INCDIR) -I$(AP_SDK_INC) -I$(BOOST_SDK_INC) -I$(BUAP_API_INC)
CINCLUDES += -I$(3PP_SDK_PATH)
CINCLUDES += -I$(ACE_SDK_INC) 
CINCLUDES += -I$(COREMW_SDK_INC)
CINCLUDES += -I$(AP_SDK_INC)

#LDFLAGS += -Wl,-rpath-link,$(LOTC_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(BOOST_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(LIB_XERCES_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(COREMW_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(ACE_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(3PP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB)

# Define own lib paths
LIBSDIR += -L$(AP_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(BUAP_API_LIB)  -L$(3PP_SDK_LIB)
LIBSDIR += -L$(COREMW_SDK_LIB)
# Define own Libs
LIBS += -lboost_regex -lboost_signals -lboost_thread -lacs_tra -lacs_aeh -lacs_apgcc -lcps_buap  -lpthread -lboost_system  

# Define own CFLAGS
CFLAGS += -Wall -ansi -DNDEBUG -D_SINGLE_CONN
#CFLAGS += -g -Wall -ansi -DNDEBUG
ifeq ($(COVERAGE),1)
	CFLAGS += -O0
else
	CFLAGS += -Os
endif

.PHONY: all
all: buftpd

.PHONY: buftpd
buftpd: $(OUTDIR)/$(BUFTPD_APNAME)

$(OUTDIR)/$(BUFTPD_APNAME): $(BUFTPD_OBJ)
	$(SILENT)$(ECHO) 'Linking $@... '
	$(CC) -o $(BUFTPD_APEXE) $(BUFTPD_OBJ) $(LIBSDIR) $(LIBS) $(LDFLAGS)
	$(call stripp,$(BUFTPD_APNAME))
	$(SILENT)$(ECHO) '$(BUFTPD_APEXE) is created and symbol is stripped'
	$(NEW_LINE)

# Include dependecies in order to build when header file changes
DEPS = $(BUFTPD_OBJ:.o=.d)
ifeq ($(MAKECMDGOALS),all)
	-include $(DEPS)
else
	ifeq ($(MAKECMDGOALS),)
		-include $(DEPS)
	endif
endif

.PHONY: clean
clean:
	$(SILENT)$(RM) -r $(OBJDIR)/*

.PHONY: distclean
distclean: clean
	$(SILENT)$(RM) -r $(OUTDIR)/$(BUFTPD_APNAME)
