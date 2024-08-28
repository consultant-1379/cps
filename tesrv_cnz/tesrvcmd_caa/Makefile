# **********************************************************************
#
# Short description:
# Makefile for TESRV command
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
#            2011-03-24   xquydao   Created
#            2011-06-08   xquydao   Updated for SDK_APG43L_PROJ_1101_001
#            2011-06-27   xquydao   Updated for makefile DR PA8 (CI & SDP)
#            2012-10-17   xvunguy   Add LDFLAGS due to updated CLH lib
#            2015-01-25   egiaava   Adapted for SLES12 and new BOOST LIB 1_54
#
# **********************************************************************

# The following variables are defined in common.mk file
#SRCDIR = src
#OBJDIR = obj
#OUTDIR = $(CXC_PATH)/bin
#INCDIR = inc

# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common

CXC_PATH = ../tesrv_cxc

TESRVLS_APNAME   = tesrvls
TESRVLS_APEXE    = $(OUTDIR)/$(TESRVLS_APNAME)
TESRVTRAN_APNAME = tesrvtran
TESRVTRAN_APEXE  = $(OUTDIR)/$(TESRVTRAN_APNAME)

CNZ_NAME ?= tesrv_cnz
CAA_NAME ?= tesrvcmd_caa
CXC_NAME ?= tesrv_cxc

# Basic paths
VOB_PATH = $(CPS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)
                                  
include $(COMMON_ROOT)/common.mk
                                 
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

# Define objs want to build
TESRVLS_OBJ = $(OBJDIR)/CPS_TESRV_tesrvls.o \
				$(OBJDIR)/CPS_TESRV_utils.o \
				$(OBJDIR)/CPS_TESRV_errors.o \
				$(OBJDIR)/CPS_TESRV_trace.o   \
				$(OBJDIR)/CPS_TESRV_common.o

TESRVTRAN_OBJ = $(OBJDIR)/CPS_TESRV_tesrvtran.o \
				$(OBJDIR)/CPS_TESRV_utils.o \
				$(OBJDIR)/CPS_TESRV_errors.o \
				$(OBJDIR)/CPS_TESRV_trace.o   \
				$(OBJDIR)/CPS_TESRV_common.o

# Define own Include paths
#CINCLUDES += -I$(INCDIR) -I/vobs/ntpes/clh_cnz/clhapi_caa/inc_ext -I$(AP_SDK_INC) 
CINCLUDES += -I$(INCDIR) -I$(AP_SDK_INC) -I$(BOOST_SDK_INC) -I$(COREMW_SDK_INC) -I$(ACE_SDK_INC)
CINCLUDES +=-I$(3PP_SDK_PATH)

# Define own lib paths
# TODO: have to change this when pes deliver lib
#LIBSDIR += -L/vobs/ntpes/clh_cnz/clhbin_cxc/bin/lib_int -L/vobs/ntpes/clh_cnz/clhbin_cxc/bin/lib_ext -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) -L$(BOOST_SDK_LIB)
LIBSDIR += -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(COREMW_SDK_LIB)

# Define own Libs
LIBS += -lpes_clhapi  -lboost_regex -lboost_filesystem -lboost_system -lboost_thread -lacs_prc\
-lacs_tra -lacs_csapi -lacs_apgcc -lpthread -lSaImmOm -lacs_dsd

# Define own LD Flags
#LDFLAGS += -Wl,--allow-shlib-undefined
LDFLAGS += -Wl,-rpath-link,$(COREMW_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(LIB_XERCES_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(ACE_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(LIB_LOGCPLUS_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(BOOST_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,/vobs/ntpes/clh_cnz/clhbin_cxc/bin/lib_int

# Define own CFLAGS
CFLAGS += -Wall -ansi -DNDEBUG
ifeq ($(COVERAGE),1)
	CFLAGS += -O0
else
	CFLAGS += -Os
endif

.PHONY: all
all: tesrvls tesrvtran

.PHONY: tesrvls
tesrvls: $(OUTDIR)/$(TESRVLS_APNAME)

.PHONY: tesrvtran
tesrvtran: $(OUTDIR)/$(TESRVTRAN_APNAME)

$(OUTDIR)/$(TESRVLS_APNAME): $(TESRVLS_OBJ)
	$(SILENT)$(ECHO) 'Linking $@... '
	$(SILENT)$(CC) -o $(TESRVLS_APEXE) $(TESRVLS_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS) 
	$(call stripp,$(TESRVLS_APNAME))
	$(SILENT)$(ECHO) '$(TESRVLS_APEXE) is created & debug symbol is stripped'
	$(NEW_LINE)

$(OUTDIR)/$(TESRVTRAN_APNAME): $(TESRVTRAN_OBJ)
	$(SILENT)$(ECHO) 'Linking $@... '
	$(SILENT)$(CC) -o $(TESRVTRAN_APEXE) $(TESRVTRAN_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS) 
	$(call stripp,$(TESRVTRAN_APNAME))
	$(SILENT)$(ECHO) '$(TESRVTRAN_APEXE) is created & debug symbol is stripped'
	$(NEW_LINE)

# Include dependecies in order to build when header file changes
DEPS = $(TESRVLS_OBJ:.o=.d) $(TESRVTRAN_OBJ:.o=.d)
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

.PHONY: distclean
distclean: clean
	$(SILENT)$(RM) -r $(OUTDIR)/$(TESRVLS_APNAME)
	$(SILENT)$(RM) -r $(OUTDIR)/$(TESRVTRAN_APNAME)

