#!/bin/bash
##
# ------------------------------------------------------------------------
#     Copyright (C) 2012 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#       tesrvls.sh
# Description:
#       A script to wrap the invocation of tesrvls from the COM CLI.
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
# Tue Oct 02 2012   xvunguy  First version
##

/usr/bin/sudo /opt/ap/cps/bin/tesrvls "$@"

exit $?
