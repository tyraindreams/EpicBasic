#!/bin/sh

find . -type f -name '*.eb' -print | while read filename; do
    echo "; XIncludeFile \"$filename\""
    cat "$filename" | grep "^\Procedure[ \.]" | sed 's/^\Procedure\./Declare\./g' | sed 's/^\Procedure/Declare  /g'
    echo ""
done > Declare.eb
