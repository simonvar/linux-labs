#!/bin/bash

echo -n "Input time for three programs (in microseconds): "
read TIME

echo -n "Input number of lines to write for three programs: "
read LINES_TO_WRITE

#echo "Time: $TIME microseconds"
#echo "Lines: $LINES_TO_WRITE "

rm ../../files/file.txt

./lab_9_1 -n 1 -l $LINES_TO_WRITE -t $TIME &
./lab_9_1 -n 2 -l $LINES_TO_WRITE -t $TIME &
./lab_9_1 -n 3 -l $LINES_TO_WRITE -t $TIME &
