# **********************************************************************
#
#
# ********************************************************************

#the following variables are defined in common.mk file
#BLOCKDIR = $(CURDIR)
#OBJDIR   = $(BLOCKDIR)/obj
#SRCDIR   = $(BLOCKDIR)/src
#INCDIR   = $(BLOCKDIR)/inc
#TESTDIR  = $(BLOCKDIR)/test
#OUTDIR   = $(CXCDIR)/bin

CXCDIR = ../../../busrv_cxc
OWN_LIB_DIR = $(CXCDIR)/bin/lib_ext

BUPDEF_APNAME = busrv_api_test
BUPDEF_APEXE  = $(OUTDIR)/$(BUPDEF_APNAME)

# Define COMMON & BUPORT paths
COMMON_DIR     = ../../../busrv_adm_caa/common
COMMON_INCDIR  = $(COMMON_DIR)/inc
COMMON_OBJDIR  = $(COMMON_DIR)/obj
COMMON_LIB     = $(COMMON_OBJDIR)/libcommon.a
COMMON_SRCDIR = $(COMMON_DIR)/src

BUPROT_DIR     = ../../../busrv_adm_caa/buprot
BUPROT_INCDIR  = $(BUPROT_DIR)/inc
BUPROT_OBJDIR  = $(BUPROT_DIR)/obj
BUPROT_LIB     = $(BUPROT_OBJDIR)/libbuprot.a
BUPROT_SRCDIR = $(BUPROT_DIR)/src

BUSRV_DIR     	= ../../../busrv_api_caa
BUSRV_INCDIR	= $(BUSRV_DIR)/inc_ext

CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common
 
include ../../../IO_Developments/Tools/linux_conf_file/common.mk

# Define objs want to build
BUPDEF_OBJ = $(OBJDIR)/busrv_api_test.o
				      
# Define own Include paths
CINCLUDES += -I$(INCDIR) -I$(BUPROT_INCDIR) -I$(COMMON_INCDIR) -I$(BUSRV_INCDIR) -I$(BOOST_SDK_INC) -I$(AP_SDK_INC)
#CINCLUDES += -I$(INCDIR) 

# Define own lib paths
#LIBSDIR += -L$(BOOST_SDK_LIB) -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)
LIBSDIR += -L$(AP_SDK_LIB) -L$(OWN_LIB_DIR) -L$(BOOST_SDK_LIB)

# Define own Libs 
LIBS += -lcps_sbc -lacs_csapi -lboost_system -lboost_thread -lboost_regex

# Define own CFLAGS
ifeq ($(COVERAGE),1)
	CFLAGS += -O0
else
	CFLAGS += -Os
endif
#CFLAGS += -g -Wall -ansi -DNDEBUG -DLOCAL_BUILD
CFLAGS += -g -Wall -ansi -DNDEBUG -std=c++0x

.PHONY: all

all: $(OUTDIR)/$(BUPDEF_APNAME)

#.PHONY: buprot
#buprot:
# These command will be executed in another shell (not the current shell)
#	cd $(BUPROT_DIR); $(MAKE) $(MFLAGS) all

#.PHONY: common
#common:
# These command will be executed in another shell (not the current shell)
#	cd $(COMMON_DIR); $(MAKE) $(MFLAGS) all
   
$(OUTDIR)/$(BUPDEF_APNAME): $(BUPDEF_OBJ)
	$(SILENT)$(ECHO) 'Creating $(OUTDIR)/$(BUPDEF_APNAME)'
#	$(SILENT)$(CC) -o $(BUPDEF_APEXE) $(BUPDEF_OBJ) $(LIBSDIR) $(LIBS) $(BUPROT_LIB) $(COMMON_LIB)
	$(SILENT)$(CC) -o $(BUPDEF_APEXE) $(BUPDEF_OBJ) $(LIBSDIR) $(LIBS)
	$(SILENT)$(ECHO) '$(OUTDIR)/$(BUPDEF_APNAME) is created'
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

