# **********************************************************************
#
# Short description:
# Makefile template for CCH service binary
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
#           
# **********************************************************************

#BEGIN : GIT INTRO
CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common

CNZ_NAME ?= clog_cnz
CAA_NAME ?= clogcmd_caa
CXC_NAME ?= clog_cxc

# Basic paths
VOB_PATH = $(CPS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)
                                  
include $(CXC_PATH)/include.mk
                                 
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

#include /vobs/IO_Developments/Tools/linux_conf_file/common.mk

CAA_NAME = cmdlls
#CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)

# here you can add own lib paths
LIBSDIR  +=

CFLAGS += -fPIC -fmessage-length=0 -Wstrict-overflow=0 -Wno-ignored-qualifiers -Wno-empty-body
CCOVFLAGS += -fprofile-arcs -ftest-coverage

ifeq ($(COVERAGE),1)
CFLAGS += -fprofile-arcs -ftest-coverage --coverage
#CINCLUDES += -Icsprot/inc
GCOV_FLAGS =
LIBS += -lgcov
endif

LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB) -Wl,-rpath-link,$(COREMW_SDK_LIB) -Wl,-rpath-link,$(SEC_SDK_LIB) -Wl,-rpath-link,$(ACE_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB) -Wl,-rpath-link,$(BOOST_SDK_LIB) -Wl,-rpath-link,$(LIB_LOGCPLUS_SDK_LIB) -Wl,-rpath-link,$(LIB_XERCES_SDK_LIB)

CMDLLS_APNAME = cmdlls
CMDLLS_DEBUG = cmdlls_dbg
CMDLLS_APBIN = $(OUTDIR)/$(CMDLLS_APNAME)
CMDLLS_SRC = CPS_CLOG_CommandArg.cpp CPS_CLOG_CommandExitCodes.cpp CPS_CLOG_cmdlls.cpp CPS_CLOG_TimeDate.cpp \
CPS_CLOG_KeySet.cpp CPS_CLOG_CommandImp.cpp CPS_CLOG_ReadDirectory.cpp

CMDLLS_OBJ = $(CMDLLS_SRC:.cpp=.obj)

# here you can add own Include paths and/or other includes
LIBSDIR_BEFORE += 

CINCLUDES += -I inc 
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(LOTC_SDK_INC)

LIBSDIR += -L$(LIBAPI_EXT_DIR) -L$(LIBAPI_INT_DIR) 
LIBSDIR += -L$(AP_SDK_LIB) -L$(ACE_SDK_LIB)

LIBS += -lrt -lpthread -lACE -lacs_csapi
 

#OBJ = $(SOURCES:.cpp=.obj)
#OBJFILES = $(addprefix $(OBJDIR)/,$(OBJ))

#DEPS := $(SOURCES:.cpp=.d)
#ifneq ($(MAKECMDGOALS),clean)
#	-include $(DEPS)
#endif

vpath %.cpp $(SRCDIR) #../cchapi_caa/src ../cchcmd_caa/common/src common/src
vpath %.h $(INCDIR) #cchprot/inc ../cchapi_caa/inc_int ../cchapi_caa/inc_ext
vpath %.obj $(OBJDIR)
#temporary added below
#vpath %.cpp cchprot/src

#Create all
.PHONY: all
all: $(CMDLLS_APNAME) 

#Create binary
.PHONY: $(CMDLLS_APNAME)
$(CMDLLS_APNAME): fs $(CMDLLS_OBJ)
	$(NEW_LINE)
	$(SILENT)$(ECHO) 'Creating executable : $(CMDLLS_APNAME)'
	$(CC) $(LDFLAGS) $(LIBSDIR) $(LIBS) $(addprefix $(OBJDIR)/,$(CMDLLS_OBJ)) -o $(CMDLLS_APBIN)
	$(call stripp,$(CMDLLS_APNAME))
	$(NEW_LINE)
	

		
# Perform prebuild actions like ClearCase checkouts
	$(NEW_LINE)

	
# ClearCase checkins
.PHONY: checkin
checkin:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Checking in CCH srv binary $(CMDLLS_APBIN)...'
	$(SILENT) if [ '$(strip $(findstring CHECKEDOUT , $(shell $(CLEARTOOL) ls $(CMDLLS_APBIN))))' = 'CHECKEDOUT' ]; then cleartool ci -nc $(CMDLLS_APBIN); else echo 'Binary is not checked out!'; fi
	$(NEW_LINE)
	
	
# Undo checkouts
.PHONY: uncheckout
uncheckout:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Undoing checkout of CCH srv binary $(CMDLLS_APBIN)...'
	$(SILENT) if [ '$(strip $(findstring CHECKEDOUT , $(shell $(CLEARTOOL) ls $(CMDLLS_APBIN))))' = 'CHECKEDOUT' ]; then cleartool uncheckout -rm $(CMDLLS_APBIN); else echo 'Binary is not checked out!'; fi
	$(NEW_LINE)

	
#Clean build files
.PHONY: clean
clean:
	$(SILENT)$(ECHO) 'Cleaning SRV build files...'
	$(SILENT)$(RM) -f $(OBJDIR)/*

#Clean binary files
.PHONY: distclean
distclean: clean
	$(SILENT)$(ECHO) 'Cleaning SRV binary files: $(CMDLLS_APNAME)'
	$(SILENT)$(RM) -f $(OUTDIR)/$(CMDLLS_APNAME)
	

# Perform chain of code analysis tools - add more?
.PHONY: metrics
metrics : cccc lint

# CCCC
# C and C++ Code Counter
.PHONY: cccc
cccc:
	$(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
	$(CCCC) $(wildcard $(INCDIR)/*) 
	$(CCCC) $(wildcard $(SRCDIR)/*) 

# LINT
# Splint is a tool for static code analysis
.PHONY: lint
lint:
	$(SILENT)$(ECHO) 'LINT file: $@'
	$(LINT) $(wildcard $(INCDIR)/*) 
	$(LINT) $(wildcard $(SRCDIR)/*)     

# Extract debug symbols
.PHONY: symbols
symbols:
	$(SILENT)$(ECHO) 'Extracting symbols from $(CMDLLS_APNAME)'
	$(SILENT)$(RM) $(OUTDIR)/$(CMDLLSAPNAME).dbg
	$(SILENT)$(OBJCOPY) --only-keep-debug $(OUTDIR)/$(CMDLLS_APNAME) $(OUTDIR)/$(CMDLLS_APNAME).dbg
	$(SILENT)$(OBJCOPY) --strip-debug $(OUTDIR)/$(CMDLLS_APNAME)
	$(SILENT)$(OBJCOPY) --add-gnu-debuglink=$(OUTDIR)/$(CMDLLS_APNAME).dbg $(OUTDIR)/$(CMDLLS_APNAME)
	readelf -n $(OUTDIR)/$(CMDLLS_APNAME)
#TODO: RENAME AND MOVE ACCORDING TO DR SS5
	$(SILENT)$(MV) $(OUTDIR)/$(CMDLLS_APNAME).dbg $(CXC_PATH)/debug/SymbolFiles/$(CMDLLS_APNAME).debug
	
		
#Create dependencies
.PHONY: depend
depend: $(SRCFILES)
	makedepend $(CINCLUDES) $^

#Create obj folder if not existing
.PHONY: fs
fs:
	if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi

## DO NOT DELETE THIS LINE -- make depend needs it

