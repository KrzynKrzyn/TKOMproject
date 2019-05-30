#!/bin/bash

#############################################################################
# Testing whether programme ends gracefully on variety of different errors. #
#############################################################################

binary=../bin/TKOM
temporary=$(mktemp)

if [ ! -e $binary ]
then
	echo Make TKOM before testing!
	exit 1
fi

good_tests=true;

for filename in $(find Tests/ -name '*.c')
do
	$binary $filename > $temporary	
	test_return=$?
	error=`cat $temporary | grep -o "Error: .*$"`
	echo -e "$filename returned: $test_return \t $error"
	if [ "$test_return" -eq 1 ]
	then
		echo "$filename successfully completed!"
	else
		echo "$filename failed!"
		good_tests=false;
	fi
	
	echo $newline
done

if [ $good_tests = false ]
then
	echo "Some tests failed!"
else
	echo "All tests successfuly completed!"
fi

rm $temporary
