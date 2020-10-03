#!/bin/bash

YML_FILES=`find test -name "*.yml"`
TEST_NUMBER=1
TEST_VARIANT=0

SEED=`printf '0x%02x' $(((RANDOM<<15)|RANDOM))`

if [ $# -eq 1 ];
then
    echo "Using user specified seed = $1"
    SEED=$1
fi

if [ -d test/fuzz ]
then
    pushd .
    cd test/fuzz
    rm -rf *
    popd
else
    mkdir test/fuzz
fi

for F in $YML_FILES
do
    DWT_FILE=`echo $F | sed 's/\.yml/.dwt'/g`
    OUT_FILE=`echo $F | sed 's/\.yml/.out'/g`
    ERR_FILE=`echo $F | sed 's/\.yml/.err'/g`
    TEST_NAME=$(basename $F)
    TEST_NAME=`echo $TEST_NAME | sed 's/\.yml/'/g`
    NAME_STEM=`echo $TEST_NAME | sed 's/_tc.*/'/g`

    for ((i=1;i<=100;i++));
    do
        NEW_NAME="${NAME_STEM}_fuzz_tc_$TEST_NUMBER"
        mkdir test/fuzz/$NEW_NAME
        echo -ne "$TEST_NAME ~> $NEW_NAME                                     \r"

        cat $F | sed "s/$TEST_NAME/$NEW_NAME/g" > test/fuzz/${NEW_NAME}/${NEW_NAME}.yml
        tools/fuzzer $DWT_FILE $SEED > test/fuzz/${NEW_NAME}/${NEW_NAME}.dwt
		cp $OUT_FILE test/fuzz/${NEW_NAME}/${NEW_NAME}.out
		cp $ERR_FILE test/fuzz/${NEW_NAME}/${NEW_NAME}.err

        TEST_NUMBER=$((TEST_NUMBER+1))
    done
    echo
done

echo "fuzzgen used seed = $SEED"

