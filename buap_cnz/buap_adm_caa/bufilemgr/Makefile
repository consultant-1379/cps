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
#            2011-07-16   xchihoa   Created
#	         2011-09-12   xngudan   Updated
#
# **********************************************************************
CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common

include $(COMMON_ROOT)/common.mk

DEBUG = 1
CFLAGS += -g
CXCDIR = ../../buap_cxc
COMMON_DIR = ../common
COMMON_OBJ_DIR = $(COMMON_DIR)/obj
COMMON_INC_DIR = $(COMMON_DIR)/inc
COMMON_SRC_DIR = $(COMMON_DIR)/src
BUAP_API_INC_DIR  = ../../buap_api_caa/inc

### Define include dir
CINCLUDES += -I$(INCDIR)

#Common files
CINCLUDES += -I$(COMMON_INC_DIR)
CINCLUDES += -I$(BUAP_API_INC_DIR)
CINCLUDES += -I$(AP_SDK_INC) -I$(BOOST_SDK_INC) -I$(ACE_SDK_INC) -I$(AP_SDK_INC) -I$(3PP_SDK_PATH) -I$(BOOST_SDK_INC) -I$(COREMW_SDK_INC)

#CINCLUDES += -I./sdk/inc

### Define link lib dir
LIBSDIR += -L$(CXCDIR)/bin
LIBSDIR  += -L$(AP_SDK_LIB)

LIBSDIR += -L$(BOOST_SDK_LIB) -L$(ACE_SDK_LIB)  -L$(COREMW_SDK_LIB)  -L$(AP_SDK_LIB) -L$(BOOST_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(3PP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(BOOST_SDK_LIB)
 

### here you can add own libs 
LIBS += -lrt -lpthread -lacs_apgcc -lacs_jtp -lacs_tra -lfms_cpf -lboost_thread 
LIBS=-lSaCkpt -lACE -lSaImmOm -lSaImmOi -lSaAmf -lSaLog -lSaNtf -lopensaf_core -lSaClm -lacs_aeh -lpthread -lboost_system

#-lfms_cpf

.phony: all clean distclean cppcheck

BUFILEMGR_OBJS =	$(OBJDIR)/CPS_BUAP_FILEMGR_Main.o \
		$(OBJDIR)/CPS_BUAP_FILEMGR.o \
		$(OBJDIR)/CPS_BUAP_MSG_text.o \
		$(OBJDIR)/CPS_BUAP_TEXT.o \
		$(OBJDIR)/CPS_BUAP_FILEMGR_Service.o
		

COMMON_OBJS = $(COMMON_OBJ_DIR)/CPS_BUAP_Server.o \
		$(COMMON_OBJ_DIR)/CPS_BUAP_Trace.o \
		$(COMMON_OBJ_DIR)/CPS_BUAP_DateTime.o \
		$(COMMON_OBJ_DIR)/CPS_BUAP_EventReporter.o \
		$(COMMON_OBJ_DIR)/CPS_BUAP_Events.o \
		$(COMMON_OBJ_DIR)/CPS_BUAP_Exception.o \
		$(COMMON_OBJ_DIR)/CPS_BUAP_Buffer.o \
		$(COMMON_OBJ_DIR)/CPS_BUAP_MSG.o \
		$(COMMON_OBJ_DIR)/CPS_BUAP_Conversation.o 
				
				
BUFILEMGR_APNAME = cps_buapfilemgrd

BUFILEMGR_APEXE = $(OUTDIR)/$(BUFILEMGR_APNAME)

.PHONY: all
all: 	$(OUTDIR)/$(BUFILEMGR_APNAME)	

$(OUTDIR)/$(BUFILEMGR_APNAME): $(SDK_OBJS) $(COMMON_OBJS) $(BUFILEMGR_OBJS) 
	$(SILENT)$(ECHO) "Linking $@..."
	$(SILENT)$(CC) -o $(BUFILEMGR_APEXE) $(LDFLAGS) $(BUFILEMGR_OBJS) $(COMMON_OBJS) $(LIBSDIR) $(LIBS)
	$(NEW_LINE)

	
$(COMMON_OBJ_DIR)/%.o: $(COMMON_SRC_DIR)/%.cpp
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

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(BUFILEMGR_APNAME)
	
.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.o
	$(RM) -r $(OBJDIR)/*.d
	$(RM) -r $(COMMON_OBJ_DIR)/*.o
	$(RM) -r $(COMMON_OBJ_DIR)/*.d

