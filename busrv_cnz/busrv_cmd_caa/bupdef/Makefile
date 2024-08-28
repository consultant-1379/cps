# **********************************************************************
#
# Short description:
# Makefile for bupdef command
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
#            2011-02-21   xquydao   Created & updated for SDK_APG43L_PROJ_0504_003
#            2011-03-17   xquydao   Updated for SDK_APG43L_PROJ_0504_006
#            2011-06-22   xquydao   Updated for makefile DR PA8 (CI & SDP)
#            2011-09-15   xdtthng   Moving elements to work with FMS
#            2012-05-08   xquydao   Updated for stripping debug symbol
#
# ********************************************************************

#the following variables are defined in common.mk file
#BLOCKDIR = $(CURDIR)
#OBJDIR   = $(BLOCKDIR)/obj
#SRCDIR   = $(BLOCKDIR)/src
#INCDIR   = $(BLOCKDIR)/inc
#TESTDIR  = $(BLOCKDIR)/test
#OUTDIR   = $(CXCDIR)/bin

CXCDIR = ../../busrv_cxc

BUPDEF_APNAME = bupdef
BUPDEF_APEXE  = $(OUTDIR)/$(BUPDEF_APNAME)

# Define COMMON & BUPORT paths
COMMON_DIR     = ../../busrv_adm_caa/common
COMMON_INCDIR  = $(COMMON_DIR)/inc
COMMON_OBJDIR  = $(COMMON_DIR)/obj
COMMON_LIB     = $(COMMON_OBJDIR)/libcommon.a
COMMON_SRCDIR = $(COMMON_DIR)/src

BUPROT_DIR     = ../../busrv_adm_caa/buprot
BUPROT_INCDIR  = $(BUPROT_DIR)/inc
BUPROT_OBJDIR  = $(BUPROT_DIR)/obj
BUPROT_LIB     = $(BUPROT_OBJDIR)/libbuprot.a
BUPORT_SRCDIR = $(BUPROT_DIR)/src
AP_LOCAL_AFP = /vobs/cm4ap/ntaes/afp_cnz/afp_cxc/bin/lib_ext 
AP_LOCAL_OHI = /vobs/cm4ap/ntaes/ohi_cnz/ohi_cxc/bin/lib_ext 

CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk

# Define objs want to build
BUPDEF_OBJ = $(OBJDIR)/cps_bupdef.o 
				      
# Define own Include paths
CINCLUDES += -I inc -I /vobs/cm4ap/ntacs/apgcc_cnz/apgccapi_caa/OMHandling/inc_ext -I /vobs/cm4ap/ntacs/apgcc_cnz/apgccapi_caa/OMHandling/inc_int -I /vobs/cm4ap/ntacs/apgcc_cnz/apgccapi_caa/ha/app_mngr/inc
CINCLUDES += -I$(INCDIR) -I$(BUPROT_INCDIR) -I$(COMMON_INCDIR) -I$(BOOST_SDK_INC) -I$(AP_SDK_INC) -I$(3PP_SDK_PATH) -I$(COREMW_SDK_INC) -I$(ACE_SDK_INC)

LDFLAGS += -g
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB) -Wl,-rpath-link,$(ACE_SDK_LIB) -Wl,-rpath-link,$(COREMW_SDK_LIB) -Wl,-rpath-link,$(SEC_SDK_LIB) 
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB) -Wl,-rpath-link,$(BOOST_SDK_LIB) -Wl,-rpath-link,$(LIB_XERCES_SDK_LIB)

# Define own lib paths
LIBSDIR += -L$(BOOST_SDK_LIB) -L$(AP_SDK_LIB) -L$(COREMW_SDK_LIB) -L$(SEC_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(LOTC_SDK_LIB) -L$(LIB_XERCES_SDK_LIB) -L$(AP_LOCAL_AFP) -L$(AP_LOCAL_OHI) -L$(3PP_SDK_LIB)

# Define own Libs 
LIBS += -lgcc_s -lstdc++ -lrt -lpthread -lACE -lboost_regex -lacs_apgcc -lacs_tra -lacs_csapi -lacs_aeh -lfms_cpf -lacs_alog -lcap -lsec_crypto_api -lsec_credu_api -lacs_dsd -lacs_prc -laes_ohi -laes_afp -laes_gcc -laes_dbo -licuuc -licudata -licui18n
LIBS +=  -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf -lboost_serialization -lboost_filesystem -lboost_system -lboost_thread
#LIBS +=  -lpthread -lACE -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf -lopensaf_core
LIBS += -lboost_regex -lboost_system -lboost_thread -lacs_apgcc -lacs_tra -lacs_csapi -lacs_aeh -lfms_cpf

# Define own CFLAGS
ifeq ($(COVERAGE),1)
	CFLAGS += -O0
else
	CFLAGS += -Os
endif
#CFLAGS += -g -Wall -ansi -DNDEBUG -DLOCAL_BUILD
CFLAGS += -g -Wall -ansi -DNDEBUG 

.PHONY: all
all: $(OUTDIR)/$(BUPDEF_APNAME)

.PHONY: buprot
buprot:
# These command will be executed in another shell (not the current shell)
	cd $(BUPROT_DIR); $(MAKE) $(MFLAGS) all

.PHONY: common
common:
# These command will be executed in another shell (not the current shell)
	cd $(COMMON_DIR); $(MAKE) $(MFLAGS) all
   
$(OUTDIR)/$(BUPDEF_APNAME): $(BUPDEF_OBJ) buprot common
	$(SILENT)$(ECHO) 'Linking $@... '
	$(SILENT)$(CC) -o $(BUPDEF_APEXE) $(BUPDEF_OBJ) $(COMMON_LIB) $(BUPROT_LIB) $(LIBSDIR) $(LIBS)
	$(SILENT)$(ECHO) '$(BUPDEF_APEXE) is created'
	$(call stripp,$(BUPDEF_APNAME))
	$(SEPARATOR_STR)
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

# this rule is defined in common.mk
#$(OBJDIR)/%.o: $(SRCDIR)/%.cpp

.PHONY: clean
clean:
	cd $(BUPROT_DIR); $(MAKE) $(MFLAGS) clean;
	cd $(COMMON_DIR); $(MAKE) $(MFLAGS) clean;
	$(SILENT)$(RM) -r $(OBJDIR)/*

.PHONY: distclean
distclean: clean
	$(SILENT)$(RM) -r $(OUTDIR)/$(BUPDEF_APNAME)
	$(SILENT)$(RM) -r $(DEBUGDIR)/$(BUPDEF_APNAME)_dbg

