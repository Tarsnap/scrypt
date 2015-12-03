#!/bin/sh


# make sure we have the files and are runnning inside the test directory
PDIR=$(pwd)

f1="test_scrypt"
f2="test_scrypt.good"
f3="test_scrypt.sh"

EXISTS=false

if [ -e ${PDIR}/${f1} ] && [ -e ${PDIR}/${f2} ] && [ -e ${PDIR}/${f3} ];
then
    EXISTS=true
fi

if [ ${EXISTS} == false ];
then
    echo "One or more of the following files is missing:"
    echo "  ${PDIR}/${f1}"
    echo "  ${PDIR}/${f2}"
    echo "  ${PDIR}/${f3}"
    echo "Please correct and rerun test_scrypt.sh"
    exit 2
fi

# if we got here, run the tests

./test_scrypt > ./test_scrypt.log
if cmp -s ./test_scrypt.log ./test_scrypt.good; then
	rm ./test_scrypt.log
	echo "TESTS PASSED!"
	exit 0
else
	echo "TESTS FAILED!"
	exit 3
fi
