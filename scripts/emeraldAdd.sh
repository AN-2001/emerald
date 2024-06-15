# !/bin/bash

if test $1; then
    emerald add `date -d $1 +%d/%m/%y`
else
    emerald add 
fi

