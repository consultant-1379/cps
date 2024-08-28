# **********************************************************************
#
# Short description:
# Makefile for cps_buap_buloader
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
#            2010-11-18   xchihoa   Created
#
# **********************************************************************
CURDIR = $(shell pwd)
REPO_NAME = cps
CPS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(CPS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk

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
CINCLUDES += -I$(AP_SDK_INC)

### Define link lib dir
LIBSDIR += -L$(CXCDIR)/bin
LIBSDIR  += -L$(AP_SDK_LIB)

### here you can add own libs 
LIBS = -lrt -lpthread -lacs_apgcc -lacs_jtp -lacs_tra  
#-lACS_JTP

BULOADER_OBJS =	$(OBJDIR)/CPS_BUAP_Loader_Main.o \
		$(OBJDIR)/CPS_BUAP_Loader.o \
		$(OBJDIR)/CPS_BUAP_LoadingSession.o \
		$(OBJDIR)/CPS_BUAP_Loader_Service.o

COMMON_OBJS = 	$(COMMON_OBJ_DIR)/CPS_BUAP_Buffer.o \
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
		$(COMMON_OBJ_DIR)/CPS_BUAP_MSG.o \
		$(COMMON_OBJ_DIR)/CPS_BUAP_OPT_UINT32.o \
		$(COMMON_OBJ_DIR)/CPS_BUAP_OPT_UINT8.o \
		$(COMMON_OBJ_DIR)/CPS_BUAP_RPF.o \
		$(COMMON_OBJ_DIR)/CPS_BUAP_Server.o \
		$(COMMON_OBJ_DIR)/CPS_BUAP_Trace.o \
		$(COMMON_OBJ_DIR)/CPS_BUAP_UINT32.o \
		$(COMMON_OBJ_DIR)/CPS_BUAP_UINT8.o
				

				
## Build instructions
#
BULOADER_APNAME = cps_buaploaderd

BULOADER_APEXE = $(OUTDIR)/$(BULOADER_APNAME)

#VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR) $(API_DIR) $(COMMON_SRC_DIR) $(COMMON_INC_DIR) $(COMMON_DIR)/obj

.PHONY: all
all: 	$(OUTDIR)/$(BULOADER_APNAME)	

$(OUTDIR)/$(BULOADER_APNAME): $(COMMON_OBJS) $(BULOADER_OBJS)
	$(SILENT)$(ECHO) "Linking $@..."
	$(SILENT)$(CC) -o $(BULOADER_APEXE) $(BULOADER_OBJS) $(COMMON_OBJS) $(LIBSDIR) $(LIBS)
	$(NEW_LINE)

$(COMMON_OBJ_DIR)/%.o: $(COMMON_SRC_DIR)/%.cpp
	$(SILENT)$(ECHO) "Generating dependencies for $<..."
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	$(SILENT)$(ECHO) "Compiling $<..."
	$(SILENT)$(CC) $(EXTRA_CFLAGS) $(GCOV_FLAGS) -c $(CFLAGS) $(CINCLUDES) $< -o $@
	$(NEW_LINE)
	
.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.o
	$(RM) -r $(OBJDIR)/*.d
	$(RM) -r $(COMMON_OBJ_DIR)/*.o
	$(RM) -r $(COMMON_OBJ_DIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(BULOADER_APNAME)

