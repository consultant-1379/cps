# **********************************************************************
#
# Short description:
# Makefile for cps_buap_bufilemgr
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
#            2012-01-03  xngudan   Updated
#
# **********************************************************************
CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common

include $(COMMON_ROOT)/common.mk

#DEBUG = 1
CFLAGS += -g
CXCDIR = ../../buap_cxc
COMMON_DIR = ../common
COMMON_OBJ_DIR = $(COMMON_DIR)/obj
COMMON_INC_DIR = $(COMMON_DIR)/inc
COMMON_SRC_DIR = $(COMMON_DIR)/src

BUFILEMGR_DIR = ../bufilemgr
BUFILEMGR_OBJ_DIR = $(BUFILEMGR_DIR)/obj
BUFILEMGR_INC_DIR = $(BUFILEMGR_DIR)/inc
BUFILEMGR_SRC_DIR = $(BUFILEMGR_DIR)/src

BULOADER_DIR = ../buloader
BULOADER_OBJ_DIR = $(BULOADER_DIR)/obj
BULOADER_INC_DIR = $(BULOADER_DIR)/inc
BULOADER_SRC_DIR = $(BULOADER_DIR)/src

BUPARMGR_DIR = ../buparmgr
BUPARMGR_OBJ_DIR = $(BUPARMGR_DIR)/obj
BUPARMGR_INC_DIR = $(BUPARMGR_DIR)/inc
BUPARMGR_SRC_DIR = $(BUPARMGR_DIR)/src

BUAP_API_INC_DIR  = ../../buap_api_caa/inc

### Define include dir
CINCLUDES += -I$(INCDIR)

#Common files
CINCLUDES += -I$(COMMON_INC_DIR)
CINCLUDES += -I$(BUFILEMGR_INC_DIR)
CINCLUDES += -I$(BULOADER_INC_DIR)
CINCLUDES += -I$(BUPARMGR_INC_DIR)
CINCLUDES += -I$(BUAP_API_INC_DIR)
CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(3PP_SDK_PATH)
CINCLUDES += -I$(BOOST_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)
CINCLUDES += -I./sdk/inc
#CINCLUDES += -I$(LOTC_SDK_CPP)/tr1

CFLAGS +=-fpermissive -Wno-error=unused-but-set-parameter -Wno-error=unused-variable -Wno-unused-local-typedefs

### Define link lib dir
LIBSDIR += -L$(CXCDIR)/bin
LIBSDIR  += -L$(AP_SDK_LIB)
LIBSDIR += -L$(BOOST_SDK_LIB) -L$(ACE_SDK_LIB) -L$(COREMW_SDK_LIB) -L$(LIB_XERCES_SDK_LIB)

### here you can add own libs
LIBS += -lacs_apgcc -lacs_jtp -lacs_tra -lfms_cpf -lboost_thread -lboost_regex -lSaCkpt -lACE -lSaImmOm -lSaImmOi -lSaAmf -lSaLog -lSaNtf -lSaClm -lacs_aeh -lpthread -lboost_system -licuuc

LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(3PP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(BOOST_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(LIB_XERCES_SDK_LIB)

.phony: all clean distclean cppcheck

BUFILEMGR_OBJS = $(BUFILEMGR_OBJ_DIR)/CPS_BUAP_FILEMGR.o \
                $(BUFILEMGR_OBJ_DIR)/CPS_BUAP_MSG_text.o \
                $(BUFILEMGR_OBJ_DIR)/CPS_BUAP_TEXT.o

BULOADER_OBJS = $(BULOADER_OBJ_DIR)/CPS_BUAP_Loader.o \
                $(BULOADER_OBJ_DIR)/CPS_BUAP_LoadingSession.o

BUPARMGR_OBJS = $(BUPARMGR_OBJ_DIR)/CPS_BUAP_PARMGR.o \
                  $(BUPARMGR_OBJ_DIR)/BUParamsOM.o \
                  $(BUPARMGR_OBJ_DIR)/StaticParamsOI.o \
		$(BUPARMGR_OBJ_DIR)/ClassicOI.o

COMMON_OBJS =   $(COMMON_OBJ_DIR)/CPS_BUAP_Buffer.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_Client.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_Config.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_Conversation.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_DateTime.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_EventReporter.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_Events.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_Exception.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_getopt.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_MSG_dynamic_parameters.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_MSG_last_auto_reload.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_MSG_static_parameters.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_OPT_UINT32.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_OPT_UINT8.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_RPF.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_Server.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_Trace.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_UINT32.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_UINT8.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_MSG_write_clog.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_MSG_write_dmr.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_MSG_write_reply.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_MSG.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_MSG_reladmpar.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_MSG_delayed_measures.o \
                $(COMMON_OBJ_DIR)/CPS_BUAP_MSG_command_log_references.o \
                 $(COMMON_OBJ_DIR)/BUParamsCommon.o

BUAP_OBJS =     $(OBJDIR)/CPS_BUAP_Main.o \
				$(OBJDIR)/CPS_BUAP_Service.o

BUAP_APNAME = cps_buapd

BUAP_APEXE = $(OUTDIR)/$(BUAP_APNAME)

.PHONY: all
all: $(OUTDIR)/$(BUAP_APNAME)

$(OUTDIR)/$(BUAP_APNAME): $(SDK_OBJS) $(COMMON_OBJS) $(BUFILEMGR_OBJS) $(BULOADER_OBJS) $(BUPARMGR_OBJS) $(BUAP_OBJS)
		$(SILENT)$(ECHO) "Linking $@..."
		$(SILENT)$(CC) -o $(BUAP_APEXE) $(LDFLAGS) $(BUFILEMGR_OBJS) $(BULOADER_OBJS) $(BUPARMGR_OBJS) $(COMMON_OBJS) $(LIBSDIR) $(LIBS) $(BUAP_OBJS)
		$(call stripp,$(BUAP_APNAME))
		$(NEW_LINE)

$(COMMON_OBJ_DIR)/%.o: $(COMMON_SRC_DIR)/%.cpp
		$(SILENT)$(ECHO) "Generating dependencies for $<..."
		@$(call make-depend,$<,$@,$(subst .o,.d,$@))
		$(SILENT)$(ECHO) "Compiling $<..."
		$(SILENT)$(CC) $(EXTRA_CFLAGS) $(GCOV_FLAGS) -c $(CFLAGS) $(CINCLUDES) $< -o $@
		$(NEW_LINE)

$(BUFILEMGR_OBJ_DIR)/%.o: $(BUFILEMGR_SRC_DIR)/%.cpp
		$(SILENT)$(ECHO) "Generating dependencies for $<..."
		@$(call make-depend,$<,$@,$(subst .o,.d,$@))
		$(SILENT)$(ECHO) "Compiling $<..."
		$(SILENT)$(CC) $(EXTRA_CFLAGS) $(GCOV_FLAGS) -c $(CFLAGS) $(CINCLUDES) $< -o $@
		$(NEW_LINE)

$(BULOADER_OBJ_DIR)/%.o: $(BULOADER_SRC_DIR)/%.cpp
		$(SILENT)$(ECHO) "Generating dependencies for $<..."
		@$(call make-depend,$<,$@,$(subst .o,.d,$@))
		$(SILENT)$(ECHO) "Compiling $<..."
		$(SILENT)$(CC) $(EXTRA_CFLAGS) $(GCOV_FLAGS) -c $(CFLAGS) $(CINCLUDES) $< -o $@
		$(NEW_LINE)

$(BUPARMGR_OBJ_DIR)/%.o: $(BUPARMGR_SRC_DIR)/%.cpp
		$(SILENT)$(ECHO) "Generating dependencies for $<..."
		@$(call make-depend,$<,$@,$(subst .o,.d,$@))
		$(SILENT)$(ECHO) "Compiling $<..."
		$(SILENT)$(CC) $(EXTRA_CFLAGS) $(GCOV_FLAGS) -c $(CFLAGS) $(CINCLUDES) $< -o $@
		$(NEW_LINE)

# Include dependecies in order to build when header file changes
DEPS = $(OBJS:.o=.d)
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
		$(RM) -r $(BUFILEMGR_OBJ_DIR)/*.o
		$(RM) -r $(BUFILEMGR_OBJ_DIR)/*.d
		$(RM) -r $(BULOADER_OBJ_DIR)/*.o
		$(RM) -r $(BULOADER_OBJ_DIR)/*.d
		$(RM) -r $(BUPARMGR_OBJ_DIR)/*.o
		$(RM) -r $(BUPARMGR_OBJ_DIR)/*.d

.PHONY: distclean
distclean: clean
			$(RM) -r $(OUTDIR)/$(BUAP_APNAME)
			$(RM) -r $(DEBUGDIR)/$(BUAP_APNAME)_dbg

