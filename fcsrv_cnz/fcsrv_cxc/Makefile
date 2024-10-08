# Makefile template for XXX library
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
# -----      ----------   --------  --------------------------
#            2012-09-14   xngudan   Created
#            2016-02-02   egiaava   Updated CXC
#                                                           
# **********************************************************************
 
# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common

include $(COMMON_ROOT)/common.mk 

 

CXC_NAME?=CPS_FCSRVBIN
CXC_NR?=CXC1372160_9
CXC_VER?=R1B
#CXC_OLD_VER?='R1A01'
BASE_SW_VER?=3.6.0-R1A
BLOCK_NAME = $(CXC_NAME)

CXC_BLK_NAME ?= cps_fcsrv

CURDIR = $(shell pwd)
CXCPATH=$(CURDIR)
CXCDIR=$(CXCPATH)

FX_CLEAN_TMP := rm -rf /tmp/$(USER)/*

export CXCDIR

#Define fcprot, common, fcsrv  directories.
FCSRV_ADM_CAA_DIR  = ../fcsrv_adm_caa
COMMON_DIR  = $(FCSRV_ADM_CAA_DIR)/common
FCSRV_DIR  = $(BUAP_ADM_CAA_DIR)/fcsrv
FCPROT_DIR = $(BUAP_CMD_CAA_DIR)/fcprot

DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=$(CXC_NAME)
BLOCK_VERSION=$(CXC_NAME)_$(CXC_VER)
BLOCK_SEARCH= $(COMMON_DIR)/src $(COMMON_DIR)/inc $(FCSRV_DIR)/src $(FCSRV_DIR)/inc \
			  $(FCPROT_DIR)/src $(FCPROT_DIR)/inc 
              
export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

CAA_DIRS =  $(FCSRV_ADM_CAA_DIR) 

			
# Define CPPCHECK src and inc directories
CPPCKECK_SRCS = $(COMMON_DIR)/src/*.cpp \
				$(FCSRV_DIR)/src/*.cpp \
				$(FCPROT_DIR)/src/*.cpp 
		
CPPCKECK_INCS = -I$(COMMON_DIR)/inc \
				-I$(FCSRV_DIR)/inc \
				-I$(FCPROT_DIR)/inc 
				
# Define CCCC source
CCCC_SRCS = $(COMMON_DIR)/src/* \
			$(FCSRV_DIR)/src/* \
			$(FCPROT_DIR)/src/* 

# Define SLOCCOUNT source
SLOCCOUNT_SRCS = $(COMMON_DIR) $(FCSRV_DIR) $(FCPROT_DIR) 

all clean distclean: 
	@for i in $(CAA_DIRS) ; do \
		(echo "cd $$i" ; \
		 cd $$i ; \
		 $(MAKE) $(MFLAGS) CXCDIR=$(CXCPATH) $@) ;\
	done
	$(call FX_CLEAN_TMP)

		
.PHONY: nightbuild
nightbuild: del_update distclean all rpm_package sdp_package esm_package 

.PHONY: del_update
del_update:
	$(call rpm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call sdp_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call esm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_ESM_VER),$(CXC_NR))

.PHONY: release
release: all rpm_package sdp_package esm_package tar_package documentation metrics

.PHONY: rpm_package
rpm_package:
	$(call rpm_file_creation,fcsrv.spec,$(CXC_NAME),$(CXC_NR),$(CXC_VER))

.PHONY: sdp_package
sdp_package:
	$(call sdp_creation_US3,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(CXC_OLD_VER),$(CXC_NR)) 
	$(call up_creation,$(CXC_NAME),$(CXC_NR),$(CXC_VER),$(CXC_OLD_VER),$(CXCPATH),$(BASE_SW_VER))

.PHONY: esm_package
esm_package:
	$(call esm_pkg_creation,$(CXCPATH),$(CXC_BLK_NAME),$(CXC_VER),$(BASE_SW_VER),$(CXC_NR),$(CXC_NAME))
                      
.PHONY: upload_package 
upload_package: 
	$(call upload_package,$(CXC_BLK_NAME),$(CXC_NR),$(CXC_NAME),$(CXC_VER))

.PHONY: tar_package
tar_package:
	$(call tar_creation,$(BLOCK_NAME),$(CXC_NR),$(CXC_VER))

.PHONY: documentation
documentation:
	$(call doc_creation)

.PHONY: metrics
metrics: cppcheck_m cccc_m sloccount_m
			
.PHONY: cppcheck_m
cppcheck_m:		
	$(call cppcheck_doc_creation,$(CPPCKECK_SRCS),$(CPPCKECK_INCS))
	
.PHONY: cccc_m
cccc_m:		
	$(call cccc_doc_creation,$(CCCC_SRCS))

.PHONY: sloccount_m
sloccount_m:
	$(call sloccount_doc_creation,$(SLOCCOUNT_SRCS))

.PHONY: help
help:
	$(SILENT)$(ECHO) 'Usage: make [target] [option]'
	$(SILENT)$(ECHO) 'target:'
	$(SILENT)$(ECHO) '  all                Build all targets (default)'
	$(SILENT)$(ECHO) '  release            Build all targets and create rpm, metrics'
	$(SILENT)$(ECHO) '  clean              Removes object files'
	$(SILENT)$(ECHO) '  distclean          Removes object and binary files'	
	$(SILENT)$(ECHO) '  help               Print help'
	$(SILENT)$(ECHO) 'option:'
	$(SILENT)$(ECHO) '  BUILD_VERBOSE=0|1  (0: quiet build (default), 1: verbose build)'
	$(SILENT)$(ECHO) '  COVERAGE=0|1       (0: no coverage (default), 1: coverage)'

