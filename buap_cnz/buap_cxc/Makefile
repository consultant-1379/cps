# **********************************************************************
#
# Short description:
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
#            2011-10-17   xtuangu   Created
#            2012-01-05   xngudan   Updated
#            2016-02-02   egiaava	Updated CXC NR
#                                                           
# **********************************************************************
 
# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common

include $(COMMON_ROOT)/common.mk 

 

CXC_NAME?=CPS_BUAPBIN
CXC_NR?=CXC1372153_9
CXC_VER?=R1D
#CXC_OLD_VER?='R1A01'
BLOCK_NAME = $(CXC_NAME)
CXC_BLK_NAME ?= cps_buap

BASE_SW_VER ?= 3.6.0-R1A

CURDIR = $(shell pwd)
CXCPATH=$(CURDIR)
CXCDIR=$(CXCPATH)
export CXCDIR

FX_CLEAN_TMP := rm -rf /tmp/$(USER)/*

#Define buap apps, common, buap commands directories.
BUAP_API_CAA_DIR  = ../buap_api_caa
BUAP_ADM_CAA_DIR  = ../buap_adm_caa
BUAP_CMD_CAA_DIR  = ../buap_cmd_caa
BULOADER_DIR  = $(BUAP_ADM_CAA_DIR)/buloader
BUFILEMGR_DIR  = $(BUAP_ADM_CAA_DIR)/bufilemgr
BUPARMGR_DIR  = $(BUAP_ADM_CAA_DIR)/buparmgr
COMMON_DIR  = $(BUAP_ADM_CAA_DIR)/common
BUAP_DIR  = $(BUAP_ADM_CAA_DIR)/buap
BUPPRINT_DIR = $(BUAP_CMD_CAA_DIR)/bupprint
BUPSET_DIR = $(BUAP_CMD_CAA_DIR)/bupset

DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=$(CXC_NAME)
BLOCK_VERSION=$(CXC_NAME)_$(CXC_VER)
BLOCK_SEARCH= $(BULOADER_DIR)/src $(BULOADER_DIR)/inc $(BUFILEMGR_DIR)/src $(BUFILEMGR_DIR)/inc \
			  $(BUPARMGR_DIR)/src $(BUPARMGR_DIR)/inc $(COMMON_DIR)/src $(COMMON_DIR)/inc \
			  $(BUAP_API_CAA_DIR)/src $(BUAP_API_CAA_DIR)/inc $(BUPPRINT_DIR)/src $(BUPPRINT_DIR)/inc \
			  $(BUPSET_DIR)/src $(BUPSET_DIR)/inc $(BUAP_DIR)/inc
export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

CAA_DIRS =  $(BUAP_API_CAA_DIR) \
			$(BUAP_ADM_CAA_DIR) \
			$(BUAP_CMD_CAA_DIR)
			
# Define CPPCHECK src and inc directories
CPPCKECK_SRCS = $(BULOADER_DIR)/src/*.cpp \
				$(BUFILEMGR_DIR)/src/*.cpp \
				$(BUPARMGR_DIR)/src/*.cpp \
				$(COMMON_DIR)/src/*.cpp \
				$(BUAP_API_CAA_DIR)/src/*.cpp \
				$(BUPPRINT_DIR)/src/*.cpp \
				$(BUPSET_DIR)/src/*.cpp \
				$(BUAP_DIR)/src/*.cpp
		
CPPCKECK_INCS = -I$(BULOADER_DIR)/inc \
				-I$(BUFILEMGR_DIR)/inc \
				-I$(BUPARMGR_DIR)/inc \
				-I$(COMMON_DIR)/inc \
				-I$(BUAP_API_CAA_DIR)/inc \
				-I$(BUPPRINT_DIR)/inc \
				-I$(BUPSET_DIR)/inc \
				-I$(BUAP_DIR)/inc				
				
# Define CCCC source
CCCC_SRCS = $(BULOADER_DIR)/src/* \
			$(BUFILEMGR_DIR)/src/* \
			$(BUPARMGR_DIR)/src/* \
			$(COMMON_DIR)/src/* \
			$(BUAP_API_CAA_DIR)/src/* \
			$(BUPPRINT_DIR)/src/* \
			$(BUPSET_DIR)/src/* \
			$(BUAP_DIR)/src/*

# Define SLOCCOUNT source
SLOCCOUNT_SRCS = $(BULOADER_DIR) $(BUFILEMGR_DIR) $(BUPARMGR_DIR) $(COMMON_DIR) $(BUAP_API_CAA_DIR) $(BUPPRINT_DIR) $(BUPSET_DIR) $(BUAP_DIR)

all clean distclean: 
	@for i in $(CAA_DIRS) ; do \
		(echo "cd $$i" ; \
		 cd $$i ; \
		 $(MAKE) $(MFLAGS) CXCDIR=$(CXCPATH) $@) ;\
	done
	$(call FX_CLEAN_TMP)

		
.PHONY: nightbuild
nightbuild: del_update  distclean all rpm_package sdp_package esm_package 

.PHONY: del_update
del_update:
	$(call rpm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call sdp_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call esm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_ESM_VER),$(CXC_NR))

.PHONY: release
release: all rpm_package sdp_package esm_package tar_package documentation metrics

.PHONY: rpm_package
rpm_package:
	$(call rpm_file_creation,buap.spec,$(CXC_NAME),$(CXC_NR),$(CXC_VER))

.PHONY: sdp_package
sdp_package:
	$(SILENT)$(ECHO) 'SDP packaging ...'
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
	

