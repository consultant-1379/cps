# **********************************************************************
#
# Short description:
# Makefile busrv
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
#		     2011-02-23   xtuangu   Created
#		     2011-03-03   xquydao   Updated for SDK_APG43L_PROJ_0504_003
#            2011-03-17   xquydao   Updated for SDK_APG43L_PROJ_0504_006
#            2011-06-22   xquydao   Updated for makefile DR PA8 (CI & SDP)
#            2011-09-15   xdtthng   Moving elements to work with FMS
#            2012-05-08   xquydao   Updated for stripping debug symbol
#
# ********************************************************************

# The following variables are defined in common.mk file
#SRCDIR = src
#OBJDIR = obj
#OUTDIR = $(CXCDIR)/bin
#INCDIR = inc

CXCDIR = ../../busrv_cxc

BUPIDLS_APNAME = bupidls
BUPIDLS_APEXE  = $(OUTDIR)/$(BUPIDLS_APNAME)

# Define COMMON & BUPORT paths
COMMON_DIR     = ../../busrv_adm_caa/common
COMMON_INCDIR  = $(COMMON_DIR)/inc
COMMON_OBJDIR  = $(COMMON_DIR)/obj
COMMON_LIB     = $(COMMON_OBJDIR)/libcommon.a
BUPROT_DIR     = ../../busrv_adm_caa/buprot
BUPROT_INCDIR  = $(BUPROT_DIR)/inc
BUPROT_OBJDIR  = $(BUPROT_DIR)/obj
BUPROT_LIB     = $(BUPROT_OBJDIR)/libbuprot.a
AP_LOCAL_AFP = /vobs/cm4ap/ntaes/afp_cnz/afp_cxc/bin/lib_ext
AP_LOCAL_OHI = /vobs/cm4ap/ntaes/ohi_cnz/ohi_cxc/bin/lib_ext

CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk

# Define objs want to build
BUPIDLS_OBJ = $(OBJDIR)/cps_bupidls.o
			      
# Define own Include paths
CINCLUDES += -I$(INCDIR) -I$(BUPROT_INCDIR) -I$(COMMON_INCDIR) -I$(BOOST_SDK_INC) -I$(AP_SDK_INC) -I$(3PP_SDK_PATH) -I$(COREMW_SDK_INC) -I$(ACE_SDK_INC)

# Define own lib paths
LIBSDIR += -L$(BOOST_SDK_LIB) -L$(AP_SDK_LIB) -L$(COREMW_SDK_LIB) -L$(SEC_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(LOTC_SDK_LIB) -L$(LIB_XERCES_SDK_LIB) -L$(AP_LOCAL_AFP) -L$(AP_LOCAL_OHI) -L$(3PP_SDK_LIB)

# Define own Libs 
LIBS += -lgcc_s -lstdc++ -lpthread -lboost_regex -lboost_system -lboost_thread -lacs_apgcc -lacs_tra -lacs_csapi -lacs_aeh -lfms_cpf -licuuc -licui18n -lacs_dsd -lacs_prc -laes_ohi -lsec_crypto_api -lsec_credu_api -laes_afp -laes_gcc -laes_dbo -licudata
LIBS += -lACE -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf -lboost_serialization -lboost_filesystem -lboost_system -lboost_thread

# Define own CFLAGS
ifeq ($(COVERAGE),1)
	CFLAGS += -O0
else
	CFLAGS += -Os
endif
#CFLAGS += -g -Wall -ansi -DNDEBUG -DLOCAL_BUILD
CFLAGS += -g -Wall -ansi -DNDEBUG 

.PHONY: all
all: $(OUTDIR)/$(BUPIDLS_APNAME)

.PHONY: buprot
buprot:
# These command will be executed in another shell (not the current shell)
	cd $(BUPROT_DIR); $(MAKE) $(MFLAGS) all

.PHONY: common
common:
# These command will be executed in another shell (not the current shell)
	cd $(COMMON_DIR); $(MAKE) $(MFLAGS) all
   
$(OUTDIR)/$(BUPIDLS_APNAME): $(BUPIDLS_OBJ) buprot common
	$(SILENT)$(ECHO) 'Linking $@... '
	$(SILENT)$(CC) -o $(BUPIDLS_APEXE) $(BUPIDLS_OBJ) $(COMMON_LIB) $(BUPROT_LIB)  $(LIBSDIR) $(LIBS)
	$(SILENT)$(ECHO) '$(BUPIDLS_APEXE) is created'
	$(call stripp,$(BUPIDLS_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)	

# Include dependecies in order to build when header file changes
DEPS = $(BUPIDLS_OBJ:.o=.d)
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
	$(SILENT)$(RM) -r $(OUTDIR)/$(BUPIDLS_APNAME)
	$(SILENT)$(RM) -r $(DEBUGDIR)/$(BUPIDLS_APNAME)_dbg

