#!/bin/bash

#testing files

echo ""
echo "=================: WELCOME TO THE CMSC421 AC FILE TEST :================"
echo
echo "Before we start, make sure proj2driver/files...c are built."
cd ../proj2driver
make
echo
echo "the files were made....."

echo "you must be root to run"
echo
#su -

echo "------------------------Checking fc421_block_file-----------------------------"
echo

echo
echo "**** Checking if we can cat a file in proj2doc/prelim.txt ****"
echo
cat ../proj2doc/prelim.txt
echo
echo "********* BLOCKING **********"
./fc421_block_file ../proj2docs/prelim.txt
cat ../proj2docs/prelim.txt
echo
echo

echo "------------------------Checking fc421_query-----------------------------"
echo
echo "*********** cheching file if it was accessed ************"
./fc421_query ../proj2docs/prelim.txt
echo
echo
echo "---------------Checking fc421_block_file same file ---------------------"
echo
echo "********* BLOCKING same blocked file **********"
./fc421_block_file ../proj2docs/prelim.txt
echo
echo
echo "------------------------Checking fc421_unblock_file-----------------------------"
echo
echo
echo "******* unblocking /proj2doc/prelim.txt *******"
./fc421_unblock_file ../proj2docs/prelim.txt
echo
echo
echo "------------------------Checking fc421_reset-----------------------------"
echo
echo
echo "******** blocking files for reset twice ********"
echo
echo
./fc421_block_file ../proj2docs/prelim.txt
./fc421_block_file ../proj2docs/prelim.txt
echo
echo
./fc421_reset
echo
echo "************Checking if we can lock a file not in the list***********"
./fc421_unblock_file ../proj2docs/prelim.txt
echo
echo ""
echo "============================= END OF TEST =================================="
echo ""
