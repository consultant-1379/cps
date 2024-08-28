# **********************************************************************
#
# Short description:
# Makefile for cps_buap_buparmgr
# **********************************************************************
#
# Ericsson AB 2010 All rights reserved.
# The information in this document is the property of Ericsson.
# Except as specifically authorized in writing by Ericsson, the receiver of this
# document shall keep the inforrrrrrrmation contained herein confidential and shall protect
# the same in whole or in part from disclosure and dissemination to third parties.
# Disclosure and disseminations to the receivers employees shall only be made
# on a strict need to know basis.
#
# **********************************************************************
#
# Rev        Date         Name      What
# -----      ---------    --------  --------------------------
#            2011-09-09   xchihoa   Created
#
# **********************************************************************
include /vobs/IO_Developments/Tools/linux_conf_file/common.mk 

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

LIBSDIR += -L$(BOOST_SDK_LIB)
 
### here you can add own libs 
LIBS = -lrt -lpthread -lacs_apgcc -lacs_jtp -lacs_tra -lfms_cpf -lboost_thread
#-lACS_JTP

BUPARMGR_OBJS =	$(OBJDIR)/CPS_BUAP_PARMGR.o \
		$(OBJDIR)/CPS_BUAP_PARMGR_Main.o \
		$(OBJDIR)/CPS_BUAP_PARMGR_Service.o
		
	
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
		$(COMMON_OBJ_DIR)/CPS_BUAP_MSG_command_log_references.o 
		
				
				
## Build instructions
#
BUPARMGR_APNAME = cps_buapparmgrd

BUPARMGR_APEXE = $(OUTDIR)/$(BUPARMGR_APNAME)

#VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR) $(API_DIR) $(COMMON_SRC_DIR) $(COMMON_INC_DIR) $(COMMON_DIR)/obj

.PHONY: all
all: 	$(OUTDIR)/$(BUPARMGR_APNAME)	

$(OUTDIR)/$(BUPARMGR_APNAME): $(COMMON_OBJS) $(BUPARMGR_OBJS)
	$(SILENT)$(ECHO) "Linking $@..."
	$(SILENT)$(CC) -o $(BUPARMGR_APEXE) $(BUPARMGR_OBJS) $(COMMON_OBJS) $(LIBSDIR) $(LIBS)
	$(NEW_LINE)

$(COMMON_OBJ_DIR)/%.o: $(COMMON_SRC_DIR)/%.cpp
	$(SILENT)$(ECHO) "Generating dependencies for $<..."
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	$(SILENT)$(ECHO) "Compiling $<..."
	$(SILENT)$(CC) $(EXTRA_CFLAGS) $(GCOV_FLAGS) -c $(CFLAGS) $(CINCLUDES) $< -o $@
	$(NEW_LINE)
	
.PHONY: prebuild
prebuild: 
	$(call ct_co,$(OUTDIR)/$(BUPARMGR_APNAME))

.PHONY: postbuild
postbuild:
	$(call ct_ci,$(OUTDIR)/$(BUPARMGR_APNAME))
	
.PHONY: undobuild
undobuild:
	$(call ct_unco,$(OUTDIR)/$(BUPARMGR_APNAME))
	
.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.o
	$(RM) -r $(OBJDIR)/*.d
	$(RM) -r $(COMMON_OBJ_DIR)/*.o
	$(RM) -r $(COMMON_OBJ_DIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(BUPARMGR_APNAME)


