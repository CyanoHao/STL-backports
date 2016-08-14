#! /bin/bash

source runcommand.shrc

for file in ../stl17/*.cpp
do
	rungcc 14 5 $file
	rungcc 14 4.9 $file
	runclang 14 3.5 $file
done
