#!/bin/bash

#echo -n "Input time for three programs (in microseconds): "
#read TIME

#echo -n "Input number of lines to write for three programs: "
#read LINES_TO_WRITE

#echo "Time: $TIME microseconds"
#echo "Lines: $LINES_TO_WRITE "

rm ../../files/file_part_2.txt
touch ../../files/file_part_2.txt

./lab_10_2_1 -n 1 -l 1 -t 1 > writer_1.txt &
./lab_10_2_1 -n 2 -l 1 -t 1 > writer_2.txt &
./lab_10_2_1 -n 3 -l 2 -t 1 > writer_3.txt &
./lab_10_2_1 -n 4 -l 1 -t 1 > writer_4.txt &
./lab_10_2_1 -n 5 -l 1 -t 1 > writer_5.txt &

./lab_10_2_2 -n 1 -l 1 -t 2 > reader_1.txt &
./lab_10_2_2 -n 2 -l 1 -t 2 > reader_2.txt &
./lab_10_2_2 -n 3 -l 1 -t 2 > reader_3.txt &
./lab_10_2_2 -n 4 -l 1 -t 2 > reader_4.txt &
./lab_10_2_2 -n 5 -l 1 -t 2 > reader_5.txt &
