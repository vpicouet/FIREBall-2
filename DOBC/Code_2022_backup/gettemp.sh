#!/bin/bash


today=$(date +'%y%m%d')
fname=/home/fireball2/data/$today/alltemps.csv

lastline=$(tail -n 1 $fname)
#echo ${lastline[0]}
dat=$(echo $lastline | awk '{print $1;}')
newline=$(echo $lastline | awk '{print $2;}')
time=$(echo $newline | awk -F',' '{print $1}')
#temp1=$(echo $newline | awk -F',' '{print $2}')
#temp2=$(echo $newline | awk -F',' '{print $3}')
temp1=$(echo $newline | awk -F',' '{print $3}')
temp2=$(echo $newline | awk -F',' '{print $5}')

echo $dat
echo $time
echo $temp1
echo $temp2
