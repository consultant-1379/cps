# **********************************************************************
#
# Short description:
# Makefile template for busrv_cmd_caa
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
# Rev		 Date		  Name      What
# -----      ---------    --------  --------------------------
#		     2011-03-02   xquydao   Created
#            2011-06-22   xquydao   Updated for makefile DR PA8 (CI & SDP)
#
# **********************************************************************

CMD_DIRS = bupdef bupidls bupls

.PHONY: all prebuild postbuild undobuild clean distclean
all prebuild postbuild undobuild clean distclean:
	@for i in $(CMD_DIRS) ; do \
		(echo "cd $$i" ; \
		 cd $$i ; \
		 $(MAKE) $(MFLAGS) $@) ;\
	done

