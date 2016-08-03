#! /bin/bash

function gcc6-14() {
	echo -e '\n\ntesting' $1 'with GCC ...\n\n'
	g++ -std=c++14 -Wall -Wextra -I../.. $1 -o $1.out
	./$1.out
}

function gcc5-14() {
	echo -e '\n\ntesting' $1 'with GCC ...\n\n'
	g++-5 -std=c++14 -Wall -Wextra -I../.. $1 -o $1.out
	./$1.out
}

function clang38-14() {
	echo -e '\n\ntesting' $1 'with Clang ...\n\n'
	clang++ -std=c++14 -Wall -Wextra -I../.. $1 -o $1.out
	./$1.out
}

function clang37-14() {
	echo -e '\n\ntesting' $1 'with Clang ...\n\n'
	clang++-3.7 -std=c++14 -Wall -Wextra -I../.. $1 -o $1.out
	./$1.out
}

for file in *.cpp
do
	gcc6-14 $file
	clang38-14 $file
	rm $file.out
done
