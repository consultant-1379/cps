# **********************************************************************
#
# Short description:
# Makefile FCSrv
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
#		     2010-11-10   xquydao   Created
#            2011-01-06   xquydao   Updated for SDK_APG43L_PROJ_0504_001
#            2011-01-26   xquydao   Updated for making it build when healder file changes
#		     2011-03-03   xquydao   Updated for SDK_APG43L_PROJ_0504_003
#            2011-03-17   xquydao   Updated for SDK_APG43L_PROJ_0504_006
#            2011-06-22   xquydao   Updated for makefile DR PA8 (CI & SDP)
#            2011-09-15   xdtthng   Moving elements to work with FMS
#            2015-01-25   egiaava   Adapted for SLES12 and new BOOST LIB 1_54
#
# ********************************************************************

CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk

# The following variables are defined in common.mk file
#SRCDIR = src
#OBJDIR = obj
#OUTDIR = $(CXCDIR)/bin
#INCDIR = inc

CXCDIR ?= ../../fcsrv_cxc

FCSRV_APNAME = cps_fcsrvd
FCSRV_APEXE  = $(OUTDIR)/$(FCSRV_APNAME)

# Define COMMON & FCPORT paths
COMMON_DIR     = ../common
COMMON_INCDIR  = $(COMMON_DIR)/inc
COMMON_OBJDIR  = $(COMMON_DIR)/obj
COMMON_LIB     = $(COMMON_OBJDIR)/libcommon.a
#COMMON_SRCDIR = $(COMMON_DIR)/src
FCPROT_DIR     = ../fcprot
FCPROT_INCDIR  = $(FCPROT_DIR)/inc
FCPROT_OBJDIR  = $(FCPROT_DIR)/obj
FCPROT_LIB     = $(FCPROT_OBJDIR)/libfcprot.a
#FCPORT_SRCDIR = $(FCPROT_DIR)/src

#OWN_INCDIR		= /vobs/cm4ap/sio/sio_fms/cpf_cnz/cpfapi_caa/inc_ext

# Define objs want to build
FCSRV_OBJ = $(OBJDIR)/EventReporter.o \
	$(OBJDIR)/FCPJob.o \
	$(OBJDIR)/FCPJobFactory.o \
	$(OBJDIR)/FCServer.o \
	$(OBJDIR)/FCService.o \
	$(OBJDIR)/FCSession.o \
	$(OBJDIR)/JobBase.o \
	$(OBJDIR)/FcReadGetPathJob.o \
	$(OBJDIR)/FcRemoveAllJob.o \
	$(OBJDIR)/FcSizeJob.o \
	$(OBJDIR)/FcWriteGetPathJob.o \
	$(OBJDIR)/FcRemoveJob.o \
	$(OBJDIR)/JobThread.o \
	$(OBJDIR)/main.o \
	$(OBJDIR)/UnknownJob.o
      
# Define own Include paths
CINCLUDES += -I$(INCDIR) -I$(FCPROT_INCDIR) -I$(COMMON_INCDIR) -I$(BOOST_SDK_INC) -I$(AP_SDK_INC)\
-I$(ACE_SDK_INC) -I$(COREMW_SDK_INC)

# Define own lib paths
LIBSDIR += -L$(BOOST_SDK_LIB) -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) -L$(COREMW_SDK_LIB)

#OWN_FMS = /vobs/cm4ap/sio/sio_fms/cpf_cnz/cpf_cxc/bin/lib_ext
#OWN_FMS = $(CPS_ROOT)/FCSrv_cnz/FCSrv_adm_caa/FCP

# Define own Libs 

# Use this line for FMS
LIBS += -lboost_regex -lboost_system -lboost_thread -lacs_apgcc -llog4cplus -lacs_tra -lacs_csapi\
-lpthread -lacs_aeh -lSaImmOm

# Use this line without using FMS
#LIBS += -lboost_regex -lboost_system -lboost_thread -lacs_apgcc -llog4cplus -lacs_tra -lacs_pha -lacs_csapi -lacs_aeh

# Define own CFLAGS
ifeq ($(COVERAGE),1)
	CFLAGS += -O0
else
	CFLAGS += -Os
endif
#CFLAGS += -g -Wall -ansi -DNDEBUG -DLOCAL_BUILD
CFLAGS += -g -Wall -ansi -DNDEBUG

LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(COREMW_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(LIB_XERCES_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(ACE_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(BOOST_SDK_LIB)

.PHONY: all
all: $(OUTDIR)/$(FCSRV_APNAME)

.PHONY: fcprot
fcprot:
# These command will be executed in another shell (not the current shell)
	cd $(FCPROT_DIR); $(MAKE) $(MFLAGS) all

.PHONY: common
common:
# These command will be executed in another shell (not the current shell)/vobs/cm4ap/sio/sio_fms/cpf_cnz/cpf_cxc/bin/lib_ext
	cd $(COMMON_DIR); $(MAKE) $(MFLAGS) all
   
$(OUTDIR)/$(FCSRV_APNAME): $(FCSRV_OBJ) fcprot common
	$(SILENT)$(ECHO) 'Linking $@... '
#	$(SILENT)$(CC) -o $(FCSRV_APEXE) $(FCSRV_OBJ) $(FCPROT_LIB) $(COMMON_LIB) -L$(OWN_FMS) $(LIBSDIR) $(LIBS)

# Use this line without FMS
	$(SILENT)$(CC) -o $(FCSRV_APEXE) $(FCSRV_OBJ) $(LDFLAGS) $(FCPROT_LIB) $(COMMON_LIB) $(LIBSDIR) $(LIBS)
	$(call stripp,$(FCSRV_APNAME))
	$(SILENT)$(ECHO) '$(FCSRV_APEXE) is created'
	$(NEW_LINE)	

# Include dependecies in order to build when header file changes
DEPS = $(FCSRV_OBJ:.o=.d)
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
	cd $(FCPROT_DIR); $(MAKE) $(MFLAGS) clean;
	cd $(COMMON_DIR); $(MAKE) $(MFLAGS) clean;
	$(SILENT)$(RM) -r $(OBJDIR)/*

.PHONY: distclean
distclean: clean
	$(SILENT)$(RM) -r $(OUTDIR)/$(FCSRV_APNAME)
	$(SILENT)$(RM) -r $(DEBUGDIR)/$(FCSRV_APNAME)_dbg

