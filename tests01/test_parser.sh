#!/bin/bash

#colored strings
pass="\033[1;32mPASS\033[1;39m"
error="\033[1;31mFAIL\033[1;39m"

#variables for statistics
error_num=0
pass_num=0

#nazov programu
run=run

#init
make

#start with error tests
cd tests/error_test
echo
echo -e "\033[1;39m"

echo "Tests for error code"
echo
for i in `ls`
do
    echo
    echo "Expected error:" `cat $i | grep //`
    expected_ret=`cat $i | grep // | cut -f2 -d'#' | head -n1`
    ./../../$run $i > /dev/null
    ret=$?
    if [ $ret -eq 0 ]
        then
        echo -e "TEST ERROR: $i: $error || return code : $ret"
        error_num=$(( $error_num + 1 ))
    else
        if [ $expected_ret -eq $ret ]
        then
            echo -e "TEST ERROR: $i: $pass  || return code : $ret"
            pass_num=$(( $pass_num + 1 ))
        else
            echo -e "TEST ERROR: $i: $error || return code : $ret"
            error_num=$(( $error_num + 1 ))
        fi
    fi
done

cd ../..
echo

cd tests/runtime_error
echo
echo
echo "Runtime error tests"
echo

for i in `ls | grep -v '\.in'`
do
    echo
    echo "Expected error:" `cat $i | grep //`
    expected_ret=`cat $i | grep // | cut -f2 -d'#' | head -n1`
    ./../../$run $i > /dev/null < $i.in
    ret=$?
    if [ $ret -eq 0 ]
        then
        echo -e "TEST ERROR: $i: $error || return code : $ret"
        error_num=$(( $error_num + 1 ))
    else
        if [ $expected_ret -eq $ret ]
        then
            echo -e "TEST ERROR: $i: $pass  || return code : $ret"
            pass_num=$(( $pass_num + 1 ))
        else
            echo -e "TEST ERROR: $i: $error || return code : $ret"
            error_num=$(( $error_num + 1 ))
        fi
    fi
done


cd ../..
echo

cd tests/good_test
pwd
echo
echo
echo
echo "Tests for good code"
echo
echo
for i in `ls | grep -v 'in' | grep -v 'out'`
do
    echo
    ./../../$run $i > stdout < $i.in
    ret=$?
    if [ $ret -eq 0 ]
        then
        diff stdout $i.out > should_be_empty
        if [ "`cat should_be_empty`" == "" ]
        then
            echo -e "TEST GOOD: $i: $pass  || return code : $ret"
            pass_num=$(( $pass_num + 1 ))
        else
            echo -e "TEST GOOD: $i: $error || return code : $ret"
            error_num=$(( $error_num + 1 ))
        fi
    else
        echo -e "TEST GOOD: $i: $error || return code : $ret"
        error_num=$(( $error_num + 1 ))
    fi
    rm -f should_be_empty stdout
done

cd ../..


echo
echo
echo

#destroy
make clean
echo
echo
echo "Num of errors : $error_num"
echo "Num of passes : $pass_num"
