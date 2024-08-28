#!/bin/bash
##
# ------------------------------------------------------------------------
#	Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       bupidls.sh
# Description:
#       A script to wrap the invocation of bupidls from the COM CLI.
# Note:
#	None.
##
# Usage:
#	None.
##
# Output:
#       None.
##
# Changelog:
# - Tue Oct 2 2012 - Danh Nguyen (xngudan)
#	First version.
##

/usr/bin/sudo /opt/ap/cps/bin/bupidls "$@"

exit $?
