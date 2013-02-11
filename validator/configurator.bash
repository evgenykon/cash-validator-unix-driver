#!/bin/bash
echo "-------- VALIDATOR CONFIGURATION SCRIPT --------"
echo "Select type of your validator:"
echo "1. Cashcode SM"
echo "2. Cashcode MFL"
echo "3. JCM"
read cashtype
case $cashtype in
1)
	cd ./cashcodesm/
	echo -e "INSERT INTO ``unixterm``.``config`` VALUES  (2,'validator_lib','lib_cashcodesm',NULL);" > ./record.sql
	make
	mysql --user=eff --password=eff < ./record.sql
	rm -rf lib/
	rm -f validator.c
	rm -f record.sql
	echo "Set port:"
	read port
	echo $port > settings.cfg
	;;
2)
	cd ./cashcodemfl/
	echo -e "INSERT INTO ``unixterm``.``config`` VALUES  (2,'validator_lib','lib_cashcodemfl',NULL);" > ./record.sql
	make
	mysql --user=eff --password=eff < ./record.sql
	rm -rf lib/
	rm -f validator.c
	rm -f record.sql
	echo "Set port:"
	read port
	echo $port > settings.cfg
	;;
3)
	cd ./jcm/
	echo -e "INSERT INTO ``unixterm``.``config`` VALUES  (2,'validator_lib','lib_jcm',NULL);" > ./record.sql
	make
	mysql --user=eff --password=eff < ./record.sql
	rm -rf lib/
	rm -f validator.c
	rm -f record.sql
	echo "Set port:"
	read port
	echo $port > settings.cfg
	;;
*)
	echo "Unknown type. Abort."
esac
cd ..
