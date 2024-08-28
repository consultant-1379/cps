# **********************************************************************
#
# Short description:
# Makefile for BUAP library
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
#            2010-12-16   xchihoa   Created
#            2011-08-10   xquydao   Updated to comply with Makefile rule PB2
#            2012-01-10   xquydao   Update to lib ver to 2.0.0
#            2012-05-21   xquydao   Stripped debuging symbol from lib 
#
# ********************************************************************

#the following variables are defined in common.mk file lines 369
#SRCDIR = src
#OBJDIR = obj
#OUTDIR = $(CXC_PATH)/bin
#INCDIR = ./inc

CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common

CNZ_NAME ?= buap__cnz
CAA_NAME ?= buap_api_caa
CXC_NAME ?= buap__cxc

# Basic paths
VOB_PATH = $(CPS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)
                                  
include $(COMMON_ROOT)/common.mk
                                 
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

# Define lib variables
LIB_NAME = cps_buap
LIB_REL_MAJOR ?= 2
LIB_REL_MINOR ?= 1
LIB_REL_BUILD ?= 1
LIB_VER ?= $(LIB_REL_MAJOR).$(LIB_REL_MINOR).$(LIB_REL_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VER)
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_REL_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

BUAP_BUFILE_APLIB = $(OUTDIR)/$(LIB_FILENAME)
BUAP_BUFILE_APLIB_SO = $(OUTDIR)/$(LIB_LINKER_NAME)

# Define dirs
CXC_PATH = ../buap_cxc
COMMON_DIR = ../buap_adm_caa/common
#COMMON_OBJ_DIR = $(COMMON_DIR)/obj
COMMON_INC_DIR = $(COMMON_DIR)/inc
#COMMON_SRC_DIR = $(COMMON_DIR)/src

# Define objs want to build
BUAP_BUFILE_OBJ = $(OBJDIR)/CPS_BUAP_BUFile.o \
				$(OBJDIR)/CPS_BUAP_LoaderClient.o \
				$(OBJDIR)/trace.o

#COMMON_OBJ =    $(COMMON_OBJ_DIR)/CPS_BUAP_Trace.o
#				$(COMMON_OBJ_DIR)/CPS_BUAP_MSG.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_EventReporter.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_Events.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_Buffer.o

# Define own Include paths
CINCLUDES += -I$(INCDIR) -I$(COMMON_INC_DIR) -I$(AP_SDK_INC) -I$(3PP_SDK_PATH)  -I$(ACE_SDK_INC) -I$(BOOST_SDK_INC) 
#CINCLUDES += -I$(INCDIR) -I$(AP_SDK_INC) -I$(3PP_SDK_PATH)

# Define own lib paths
LIBSDIR += -L$(AP_SDK_LIB) 

# Define own Libs
LIBS += -lfms_cpf -lacs_tra -lacs_jtp

# Define own VPATH		 
VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR)

# Define own LDFLAGS & CFLAGS
LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME)
CFLAGS += -Wall -ansi -DNDEBUG
ifeq ($(COVERAGE),1)
	CFLAGS += -O0
else
	CFLAGS += -Os
endif

.PHONY: all
all: $(OUTDIR)/$(LIB_FILENAME)

$(OUTDIR)/$(LIB_FILENAME): $(BUAP_BUFILE_OBJ)
	$(SILENT)$(ECHO) 'Creating library: $(OUTDIR)/$(LIB_FILENAME)'
	$(SILENT)$(CC) -o $(BUAP_BUFILE_APLIB) $(BUAP_BUFILE_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(LIB_FILENAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

# Include dependecies in order to build when header file changes
DEPS = $(BUAP_BUFILE_OBJ:.o=.d)
ifeq ($(MAKECMDGOALS),all)
	-include $(DEPS)
else
	ifeq ($(MAKECMDGOALS),)
		-include $(DEPS)
	endif
endif

#$(COMMON_OBJ_DIR)/%.o: $(COMMON_SRC_DIR)/%.cpp
#	$(SILENT)$(ECHO) "Generating dependencies for $<..."
#	$(call make-depend,$<,$@,$(subst .o,.d,$@))
#	$(SILENT)$(ECHO) "Compiling $<..."
#	$(SILENT)$(CC) $(EXTRA_CFLAGS) $(GCOV_FLAGS) -c $(CFLAGS) $(CINCLUDES) $< -o $@
#	$(NEW_LINE)

.PHONY: clean
clean:
	$(SILENT)$(RM) -r $(OBJDIR)/*.o
	$(SILENT)$(RM) -r $(OBJDIR)/*.d
	#     $(SILENT)$(RM) -r $(COMMON_OBJ_DIR)/*.o
	#     $(SILENT)$(RM) -r $(COMMON_OBJ_DIR)/*.d

.PHONY: distclean
distclean: clean
	$(SILENT)$(RM) -r $(OUTDIR)/$(LIB_FILENAME)

