# !/bin/bash

if test $1; then
    emerald add `date --date="$*" +%d/%m/%y`
else
    emerald add 
fi

