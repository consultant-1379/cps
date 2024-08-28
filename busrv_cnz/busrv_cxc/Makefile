# **********************************************************************
#
# Short description:
# Makefile for busrv_cxc
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
#            2011-01-19   xquydao   Created
#            2011-01-25   xquydao   Updated for rpm & cppcheck creation
#            2011-01-02   xquydao   Updated target  & 
#            2011-02-14   xquydao   Updated for SDK_APG43L_PROJ_0504_002
#            2011-03-17   xquydao   Updated for SDK_APG43L_PROJ_0504_006
#            2011-06-22   xquydao   Updated for makefile DR PA8 (CI & SDP)
#            2016-01-29   egiaava   Updated for makefile APZ23.0
#
# **********************************************************************
 
# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common

include $(COMMON_ROOT)/common.mk 

CXC_NAME    ?= CPS_BUSRVBIN
CXC_NR      ?= CXC1372151_9
CXC_VER     ?= R1F
#CXC_OLD_VER ?= R1A01

CXC_BLK_NAME ?= cps_busrv

BASE_SW_VER ?= 3.6.0-61A

FX_CLEAN_TMP := rm -rf /tmp/$(USER)/*

CURDIR  = $(shell pwd)
CXCPATH = $(CURDIR)
CXCDIR  = $(CXCPATH)

# Define BUSRV app name
BUSRV_APNAME   = cps_busrvd
BUPDEF_APNAME  = bupdef
BUPIDLS_APNAME = bupidls
BUPLS_APNAME   = bupls

CAA_DIRS = ../busrv_adm_caa \
			../busrv_cmd_caa \
			../busrv_api_caa

# Define buprot, common & busrv, bu cmds directories
BUPROT_DIR  = ../busrv_adm_caa/buprot
COMMON_DIR  = ../busrv_adm_caa/common
BUSRV_DIR   = ../busrv_adm_caa/busrv
BUPDEF_DIR  = ../busrv_cmd_caa/bupdef
BUPIDLS_DIR = ../busrv_cmd_caa/bupidls
BUPLS_DIR   = ../busrv_cmd_caa/bupls

# Define CPPCHECK src and inc directories
CPPCKECK_SRCS = $(BUPROT_DIR)/src/*.cpp \
				$(COMMON_DIR)/src/*.cpp \
				$(BUSRV_DIR)/src/*.cpp \
				$(BUPDEF_DIR)/src/*.cpp \
				$(BUPIDLS_DIR)/src/*.cpp \
				$(BUPLS_DIR)/src/*.cpp

CPPCKECK_INCS = -I$(BUPROT_DIR)/inc \
				-I$(COMMON_DIR)/inc \
				-I$(BUSRV_DIR)/inc \
				-I$(BUPDEF_DIR)/inc \
				-I$(BUPIDLS_DIR)/inc \
				-I$(BUPLS_DIR)/inc

# Define CCCC src
CCCC_SRCS = $(BUPROT_DIR)/src/* \
			$(COMMON_DIR)/src/* \
			$(BUSRV_DIR)/src/* \
			$(BUPDEF_DIR)/src/* \
			$(BUPIDLS_DIR)/src/* \
			$(BUPLS_DIR)/src/*

# Define SLOCCOUNT src
SLOCCOUNT_SRCS = $(BUPROT_DIR) $(COMMON_DIR) $(BUSRV_DIR) $(BUPDEF_DIR) $(BUPIDLS_DIR) $(BUPLS_DIR)

# TODO: no need for now, do later
DOXYGEN_OUTPUT = $(DOCDIR)/docgen
BLOCK_NAME     = $(CXC_NAME)
BLOCK_VERSION  = $(CXC_VER)
BLOCK_SEARCH  = $(BUPROT_DIR)/inc $(BUPROT_DIR)/src $(COMMON_DIR)/inc $(COMMON_DIR)/src $(BUSRV_DIR)/inc $(BUSRV_DIR)/src \
				$(BUPDEF_DIR)/inc $(BUPDEF_DIR)/src $(BUPIDLS_DIR)/inc $(BUPIDLS_DIR)/src $(BUPLS_DIR)/inc $(BUPLS_DIR)/src
				 
export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

.PHONY: all clean distclean
all clean distclean: 
	@for i in $(CAA_DIRS) ; do \
		(echo "cd $$i" ; \
		 cd $$i ; \
		 $(MAKE) $(MFLAGS) CXCDIR=$(CXCPATH) $@) ;\
		 if [ $$? != 0 ]; \
		  then $(ECHO) "Error: please see the printout!!!";\
		  exit $$?; \
		 fi;\
	done
	$(call FX_CLEAN_TMP)

.PHONY: rpm_package
rpm_package:
	$(call rpm_file_creation,busrv.spec,$(CXC_NAME),$(CXC_NR),$(CXC_VER))

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

.PHONY: nightbuild
nightbuild: del_update distclean all rpm_package sdp_package esm_package 

.PHONY: del_update
del_update:
	$(call rpm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call sdp_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call esm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_ESM_VER),$(CXC_NR))

.PHONY: release
release: all rpm_package sdp_package esm_package tar_package documentation metrics
	
.PHONY: metrics
metrics: cppcheck_m cccc_m sloccount_m

.PHONY: documentation
documentation:
	$(call doc_creation)

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

