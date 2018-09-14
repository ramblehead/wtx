#!/bin/bash

set -eu
set -o pipefail

COMMAND=`grep -E "compile-command:\s+\".*\"" BootstrapToggle.cpp|\
sed 's/\(^.*compile-command:\s*"\)\|\(\"\s*$\)//g'`

rc -c "$COMMAND"
