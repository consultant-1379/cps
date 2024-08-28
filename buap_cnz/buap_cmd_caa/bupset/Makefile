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
# -----      ---------    --------  --------------------------
#            2011-10-17   xchihoa   Created
#
# ********************************************************************

CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common

include $(COMMON_ROOT)/common.mk

CXCDIR = ../../buap_cxc
COMMON_DIR = ../../buap_adm_caa/common
COMMON_OBJ_DIR = $(COMMON_DIR)/obj
COMMON_INC_DIR = $(COMMON_DIR)/inc
COMMON_SRC_DIR = $(COMMON_DIR)/src
PARMGR_DIR = ../../buap_adm_caa/buparmgr
PARMGR_OBJ_DIR = $(PARMGR_DIR)/obj
PARMGR_INC_DIR = $(PARMGR_DIR)/inc
PARMGR_SRC_DIR = $(PARMGR_DIR)/src

CINCLUDES += -I$(INCDIR)

## # here you can add own Include paths and/or other INCDIRludes
CINCLUDES += -I$(COMMON_INC_DIR) -I$(PARMGR_INC_DIR)
CINCLUDES += -I$(AP_SDK_INC) -I$(3PP_SDK_PATH)
CINCLUDES += -I$(COREMW_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(BOOST_SDK_INC)
### Libs
LIBSDIR += -L$(CXCDIR)/bin
LIBSDIR  += -L$(AP_SDK_LIB) -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB) -L$(AP_SDK_LIB) -L$(BOOST_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(3PP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(BOOST_SDK_LIB)

## # here you can add own libs 
LIBS += -lacs_tra -lacs_jtp -lacs_apgcc -lSaCkpt -lACE -lSaImmOm -lSaImmOi -lSaAmf -lSaLog -lSaNtf -lSaClm -lacs_aeh -lpthread -lboost_system -lacs_csapi

.phony: all clean distclean

BUPSET_OBJ =  $(OBJDIR)/CPS_BUAP_BUPSET_Main.o \
				$(OBJDIR)/CPS_BUAP_BUPSET.o

COMMON_OBJS = 	$(COMMON_OBJ_DIR)/CPS_BUAP_Buffer.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_Client.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_EventReporter.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_Events.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_Exception.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_MSG.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_MSG_dynamic_parameters.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_MSG_last_auto_reload.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_MSG_static_parameters.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_Trace.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_OPT_UINT8.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_OPT_UINT32.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_UINT8.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_MSG_command_log_references.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_MSG_delayed_measures.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_DateTime.o \
				$(COMMON_OBJ_DIR)/CPS_BUAP_MSG_write_reply.o \
				 $(COMMON_OBJ_DIR)/CPS_BUAP_getopt.o \
				 $(COMMON_OBJ_DIR)/CPS_BUAP_Config.o \
                                $(COMMON_OBJ_DIR)/BUParamsCommon.o

PARMGR_OBJS =	$(PARMGR_OBJ_DIR)/BUParamsOM.o

## Build instructions
#
BUPSET_APNAME = bupset

BUPSET_APEXE = $(OUTDIR)/$(BUPSET_APNAME)

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR)

.PHONY: all
all: 	$(OUTDIR)/$(BUPSET_APNAME)

$(OUTDIR)/$(BUPSET_APNAME): $(COMMON_OBJS) $(PARMGR_OBJS) $(BUPSET_OBJ)
	$(SILENT)$(ECHO) "Linking $@..."
	$(SILENT)$(CC) -o $(BUPSET_APEXE) $(BUPSET_OBJ) $(COMMON_OBJS) $(LDFLAGS) $(PARMGR_OBJS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(BUPSET_APNAME))
	$(NEW_LINE)

$(COMMON_OBJ_DIR)/%.o: $(COMMON_SRC_DIR)/%.cpp
	$(SILENT)$(ECHO) "Generating dependencies for $<..."
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	$(SILENT)$(ECHO) "Compiling $<..."
	$(SILENT)$(CC) $(EXTRA_CFLAGS) $(GCOV_FLAGS) -c $(CFLAGS) $(CINCLUDES) $< -o $@
	$(NEW_LINE)
	
$(PARMGR_OBJ_DIR)/%.o: $(PARMGR_SRC_DIR)/%.cpp
	$(SILENT)$(ECHO) "Generating dependencies for $<..."
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	$(SILENT)$(ECHO) "Compiling $<..."
	$(SILENT)$(CC) $(EXTRA_CFLAGS) $(GCOV_FLAGS) -c $(CFLAGS) $(CINCLUDES) $< -o $@
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

.PHONY: clean
clean:  
	$(RM) -r $(OBJDIR)/*.o
	$(RM) -r $(OBJDIR)/*.d
	$(RM) -r $(COMMON_OBJ_DIR)/*.o
	$(RM) -r $(COMMON_OBJ_DIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(BUPSET_APNAME)
	$(RM) -r $(DEBUGDIR)/$(BUPSET_APNAME)_dbg
	

#ifneq ($(MAKECMDGOALS),clean)
#include $($(OBJDIR)/%.d)
#endif

