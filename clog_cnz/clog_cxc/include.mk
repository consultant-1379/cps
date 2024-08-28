# **********************************************************************
#
# Short description:
# cmdlls common.mk configuration file
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
# **********************************************************************

VOB_NAME = Gemini_CPS

#BEGIN : GIT INTRO
CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common


# Handling Eclipse clearcase paths
ifeq ($(ECLIPSE),1)
IODEV_VOB := /vobs/IO_Developments
VOB_PATH := $(CPS_ROOT)
else
IODEV_VOB = /vobs/IO_Developments
VOB_PATH = $(CPS_ROOT)
endif
CNZ_NAME = clog_cnz
CNZ_PATH = $(shell pwd)/../../$(CNZ_NAME)

CXC_NAME ?= CPS_CLOGCMDBIN

CXC_NAME_PATH ?= clog_cxc
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME_PATH)
CXCDIR = $(CNZ_PATH)/$(CXC_NAME_PATH)
CXC_NR ?= CXC1372205_3
VERSION = 2.2-0
CXC_OLD_VER ?= R1A04
CXC_VER ?= R1A02
BASE_SW_VER ?= 1.0.0-R1A05

BLOCK_NAME = CPS_CLOGCMDBIN
DOXYGENCONFIGFILE = #$(CXCDIR)/doc/clog_doxygen.cfg

include $(COMMON_ROOT)/common.mk

#LIBAPI_DIR =  $(CNZ_PATH)/clog_cxc/bin/lib_ext

#LIBAPI_EXT_DIR = $(CNZ_PATH)/clog_cxc/bin/lib_ext
#LIBAPI_INT_DIR = $(CNZ_PATH)/clog_cxc/bin/lib_int

CCCC_FLAGS += --xml_outfile=$(CCCC_OUTPUT)/cccc.xml


# Handling optimization for release builds
ifneq ($(DEBUG),1)
CFLAGS += -O3
endif
