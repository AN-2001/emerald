# !/bin/bash

if test $1; then
    emerald print `date -d $1 +%d/%m`
else
    emerald print 
fi

