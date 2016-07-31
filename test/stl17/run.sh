#! /bin/bash

function g++14() {
	echo -e '\n\ntesting' $1 'with GCC ...\n\n'
	g++ -std=c++14 -Wall -Wextra -I../.. $1 -o $1.out
	./$1.out
}

function clang++14() {
	echo -e '\n\ntesting' $1 'with GCC ...\n\n'
	g++ -std=c++14 -Wall -Wextra -I../.. $1 -o $1.out
	./$1.out
}

for file in *.cpp
do
	g++14 $file
	rm $file.out
done
