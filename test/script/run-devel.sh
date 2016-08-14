#! /bin/bash

source runcommand.shrc

for file in ../stl17/*.cpp
do
	rungcc 14 6 $file
done
