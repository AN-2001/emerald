# !/bin/bash

if test $1; then
    emerald print `date --date="$*" +%d/%m/%y`
else
    emerald print 
fi

