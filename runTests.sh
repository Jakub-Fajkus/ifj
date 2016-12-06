#!/usr/bin/env bash

FILES=`find tests -iname  'test*' | sort -n | grep txt`
FAILED_COUNTER=0
OK_COUNTER=0

for FILE in $FILES; do
    EXPECTED_RETURN_CODE=`echo $FILE | cut -d'/' -f 2 | cut -d'-' -f 2`

    printf "\n**************** RUNNING INTERPRET FOR $FILE ****************\n"
    printf "executing: ./bin/Debug/ifj %s \n" "$FILE"
    ./bin/Debug/ifj $FILE;
    RETURNED_CODE=$?
    if [ $RETURNED_CODE = $EXPECTED_RETURN_CODE ]; then
        printf "\e[34mexpected %d, got %d -> test $FILE OK\e[m" "$EXPECTED_RETURN_CODE" "$RETURNED_CODE"
        ((OK_COUNTER++))
    else
        printf "\e[31mexpected %d, got %d -> test $FILE FAILED\e[m" "$EXPECTED_RETURN_CODE" "$RETURNED_CODE"
        ((FAILED_COUNTER++))
    fi

done

printf "\e[34m\n\n\n**************** END OF TESTS ****************\n"
printf "\e[34mtests ok: %d \n\e[m" "$OK_COUNTER"
printf "\e[31mtests failed: %d \n\e[m" "$FAILED_COUNTER"
