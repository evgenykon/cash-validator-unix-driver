#!/bin/bash
if ps -A | grep shtrih > /dev/null
then
	echo "DONE"
else
	/var/www/bin/printer/shtrihmfrk/shtrih-mfk.bin start > /dev/null &
	if ps -A | grep shtrih > /dev/null
	then
		echo "RUN"
	else
		echo "FAIL"
	fi
fi
