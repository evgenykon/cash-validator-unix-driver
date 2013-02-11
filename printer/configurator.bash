#!/bin/bash
echo "-------- PRINTER CONFIGURATION SCRIPT --------"
echo "Select type of your validator:"
echo "1. Shtrih MINI FR K"
echo "2. Default (cap)"
read cashtype
case $cashtype in
1)
	cd ./shtrihmfrk/
	echo -e "INSERT INTO ``unixterm``.``config`` VALUES  (16,'printer_lib','lib_shtrihmfrk',NULL);" > ./record.sql
	make
	make printer
	mysql --user=eff --password=eff < ./record.sql
	rm -rf lib/
	rm -f printer.c
	rm -f shtrih-mini-fr-k.c
	rm -f record.sql
	sudo chmod 777 check_text.txt
	sudo ./shtrih-mfk.bin setconfig
	;;
2)
	cd ./cap/
	echo -e "INSERT INTO ``unixterm``.``config`` VALUES  (16,'printer_lib','lib_defprint',NULL);" > ./record.sql
	mysql --user=eff --password=eff < ./record.sql
	rm -f record.sql
	;;
*)
	echo "Unknown type. Abort."
esac
cd ..
