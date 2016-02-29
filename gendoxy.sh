#!/bin/bash

filename='projecthomelist.txt'
while read -r line
do
	name="$line"
	cd $name
	doxygen
	cd ..
done < "$filename"