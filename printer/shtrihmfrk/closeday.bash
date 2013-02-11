#!/bin/bash
if ps -A | grep shtrih > /dev/null
then
	echo "Service already started"
else
	/var/www/bin/printer/shtrihmfrk/run.bash
	echo "Service started"
fi
/var/www/bin/printer/shtrihmfrk/printer.bin closesmena
