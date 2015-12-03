#!/bin/sh


# make sure we have the files and are runnning inside the test directory
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PDIR=$(pwd)

f1="test_scrypt"
f2="test_scrypt.good"
f3="test_scrypt.sh"

EXISTS=false
TDIR=false

if [ ${DIR} == ${PDIR}"/test" ];
then
    TDIR=true
    if [ -e ${DIR}/${f1} ] && [ -e ${DIR}/${f2} ] && [ -e ${DIR}/${f3} ];
    then
        EXISTS=true
    fi
fi


if [ ${TDIR} == false ];
then
    echo "test_scrypt.sh must be run from the parent directory.  e.g. test/test_scrypt.sh"
    exit 1
fi

if [ ${EXISTS} == false ];
then
    echo "One or more of the following files is missing:"
    echo "  ${DIR}/${f1}"
    echo "  ${DIR}/${f2}"
    echo "  ${DIR}/${f3}"
    echo "Please correct and rerun test_scrypt.sh"
    exit 2
fi

# if we got here, run the tests

${DIR}/test_scrypt > ${DIR}/test_scrypt.log
if cmp -s ${DIR}/test_scrypt.log ${DIR}/test_scrypt.good; then
	rm ${DIR}/test_scrypt.log
	echo "TESTS PASSED!"
	exit 0
else
	echo "TESTS FAILED!"
	exit 3
fi
