# !/bin/bash

if test $1; then
    emerald print `date -d $1 +%d/%m/%y`
else
    emerald print 
fi

