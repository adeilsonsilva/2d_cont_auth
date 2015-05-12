#!/bin/bash
i=1
pics=1
max=51
j=0
while [ $i -lt $max ]
	do
	   j=1
	   mkdir s$i
	   while [ $j -lt 16 ]
		   do
				mv "$pics.png" s$i
		   		true $((j++))
				true $((pics++))
		   done
	   true $(( i++ ))
	done


