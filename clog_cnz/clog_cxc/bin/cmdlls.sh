#!/bin/bash
# ------------------------------------------------------------------------
# Copyright (C) 2013 Ericsson AB. All rights reserved.
# ------------------------------------------------------------------------
##
# Name:
#      fwprint.sh
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
# - 17/10/12 - Andrea Formica (eanform)
#       First version.
##

/usr/bin/sudo /opt/ap/cps/bin/cmdlls "$@"

exit $?
