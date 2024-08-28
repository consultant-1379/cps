# **********************************************************************
#
# Short description:
# Makefile template for busrv_adm_caa
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
#		     2010-11-17   xtuangu   Created
#
# **********************************************************************

#BUAP_SUB_DIRS = buloader bufilemgr buparmgr buap
BUAP_SUB_DIRS = buap
.PHONY: all clean distclean
all prebuild postbuild undobuild clean distclean:
	@for i in $(BUAP_SUB_DIRS) ; do \
		(echo "+++++++++++++++++" ; \
		echo "cd $$i" ; \
		cd $$i ; \
		$(MAKE) $(MFLAGS) $@ ) ; \
		echo "+++++++++++++++++" ; \
	done

