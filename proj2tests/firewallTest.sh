#!/bin/bash

#testing firewall

echo ""
echo "=================: WELCOME TO THE CMSC421 FIREWALL TEST :================"
echo
echo "Before we start, make sure proj2driver/files...c are built."
cd ../proj2driver
make
echo
echo "the files were made....."

echo "you must be root to run"
echo
#su -

echo "------------------------Checking fw421_block_port-----------------------------"
echo
echo "**** Checking if we can connect to google using port 80 ****"
nc -v -z www.google.com 80
echo
echo "*********** Blocking port 80 *********"
echo
./fw421_block_port tcp in 80
echo
echo
echo "**** Checking if we can connect to google using port 80 after blocking ****"
nc -v -z www.google.com 80
echo
echo
echo "*********** Blocking port 443 *********"
./fw421_block_port tcp in 443
echo
echo
echo "**** Checking if we can connect to google using port 80 after blocking ****"
nc -v -z www.google.com 80
echo "**** Checking if we can connect to google using port 443 after blocking ****"
nc -v -z www.google.com 443
echo
echo

echo "------------------------Checking fw421_query-----------------------------"
echo
echo "**** Checking how many times port 80 was accesed ****"
./fw421_query tcp in 80
echo
echo
echo "**** Checking if query adds on if port 80 was accesed again ****"
nc -v -z www.google.com 80
echo
./fw421_query tcp in 80
echo
echo
#loop didnt work jst iterate once
#echo "*** loop for query port 80 ***"
#echo

#myloop(){
#	for i in {1..5}
#	do
#   	nc -v -z www.google.com 80
#	done
#}

#yloop
echo
echo "**** Checking how many times port 80 was accesed ****"
./fw421_query tcp in 80
echo
echo
echo "-----------------------Checking fw421_unblock_port----------------------------"
echo
echo
echo "*********** UnBlocking port 80 ***********"
./fw421_unblock_port tcp in 80
echo
echo "**** Checking if we can connect to google using port 80 ****"
nc -v -z www.google.com 80
echo
echo
echo "*********** UnBlocking port 443 ***********"
./fw421_unblock_port tcp in 443
echo
echo "**** Checking if we can connect to google using port 443 ****"
nc -v -z www.google.com 443
echo
echo

echo "-----------------------Checking fw421_reset----------------------------"
echo
echo
echo "*********** Blocking port 80 and 443 to test reset *********"
echo
./fw421_block_port tcp in 80
./fw421_block_port tcp in 443
echo
echo
echo "**** Checking if we can connect to google using port 80 ****"
nc -v -z www.google.com 80
echo
echo "**** Checking if we can connect to google using port 443 ****"
nc -v -z www.google.com 443
echo
echo
echo "*********** Resetting *********"
./fw421_reset
echo
echo "**** Checking if we can connect to google using port 80 ****"
nc -v -z www.google.com 80
echo
echo "**** Checking if we can connect to google using port 443 ****"
nc -v -z www.google.com 443
echo
echo
echo "-----------------------Checking UDP----------------------------"
echo
echo "*** Check if we can send a UDP packet to port 6868 ***"
echo
echo -n "UPD Packet" | nc -u -w1 localhost 6868 && echo "Packet UDP Recieved"
echo
echo "*********** Blocking UDP port 6868 *********"
./fw421_block_port udp in 6868
echo
echo
echo "*** Check if we can send a UDP packet to port 6868  after block 3 times ***"
echo -n "UPD Packet" | nc -u -w1 localhost 6868 && echo "Packet UDP Recieved"
echo -n "UPD Packet" | nc -u -w1 localhost 6868 && echo "Packet UDP Recieved"
echo -n "UPD Packet" | nc -u -w1 localhost 6868 && echo "Packet UDP Recieved"
echo
echo "-----------------------Checking UDP query----------------------------"
echo
echo
echo "**** Checking how many times port udp 6868 was accesed ****"
./fw421_query udp in 6868
echo
echo
echo "-----------------------Checking UDP Unblock----------------------------"
echo
echo "************ Unblock UDP port 6868 *************"
echo
./fw421_unblock_port upd in 6868
echo
echo
echo "********** Cheching if we can send UPD packet ***********"
echo -n "UPD Packet" | nc -u -w1 localhost 6868 && echo "Packet UDP Recieved"
echo
echo
echo "-----------------------Checking UDP reset----------------------------"
echo
echo "*********** Blocking UDP port 7878 for testing reset *********"
./fw421_block_port udp in 7878
echo
echo
echo "********** Cheching if we can send UPD packet port 7878 ***********"
echo -n "UPD Packet" | nc -u -w1 localhost 7878 && echo "Packet UDP Recieved"
echo
echo "*********** Blocking UDP port 5858 for testing reset *********"
./fw421_block_port udp in 5858
echo
echo "********** Cheching if we can send UPD packet port 5858 ***********"
echo -n "UPD Packet" | nc -u -w1 localhost 5858 && echo "Packet UDP Recieved"
echo
echo
echo "******* Resetting UDP port 7878 and 5858 ******"
./fw421_reset
echo
echo
echo "******* testing if both udp port can send udp packet to 7878 and 5858 ********"
echo
echo -n "UPD Packet" | nc -u -w1 localhost 5858 && echo "Packet UDP Recieved"
echo
echo -n "UPD Packet" | nc -u -w1 localhost 7878 && echo "Packet UDP Recieved"

echo
echo
echo "============================= END OF TEST =================================="
echo ""
