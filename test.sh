#!/bin/bash

count=1

for file in "testcases"/*; do
	echo $file
	printValue=$(./main.exe < "$file" > "outputs/output$count.txt")
	count=$((count+1))
done

exit 0