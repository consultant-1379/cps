#!/bin/bash
# ------------------------------------------------------------------------
# Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#      cmdlls.sh
# Description:
#       A script to wrap the invocation of fwprint the COM CLI.
# Note:
#       None.
##
# Usage:
#       None.
##
# Output:
#       None.
##
# Changelog:
#       First version.
##

/usr/bin/sudo /opt/ap/cps/bin/cmdlls "$@"

exit $?

