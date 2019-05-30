#!/bin/bash

######################################################
# Testing whether programme ends with expected error #
######################################################

binary=../bin/TKOM
temporary=$(mktemp)

if [ ! -e $binary ]
then
	echo Make TKOM before testing!
	exit 1
fi

good_tests=true;

declare -a filename=(	"Tests/Semantic/undeclared_class.c" 
			"Tests/Semantic/undeclared_function.c"
			"Tests/Semantic/undeclared_variable.c"
			"Tests/Semantic/multiple_declaration.c"
			"Tests/Semantic/bad_constructor.c"
			"Tests/Semantic/bad_types.c"
			"Tests/Semantic/bad_return.c")

declare -a error=(	"Error: Undeclared class! (error)" 
			"Error: Undeclared function! (error(int))"
			"Error: Undeclared variable! (invalid_instance)"
			"Error: Multiple declaration! (result(int))"
			"Error: Constructor name doesn't match class name!"
			"Error: Types doesn't match!"
			"Error: Return type doesn't match function type!")
i=0
for (( i=0; i<${#filename[@]} ;i++ ))
do
	exe_result=`$binary ${filename[$i]} | grep -o "Error: .*$"`
	if [ "$exe_result" = "${error[$i]}" ]
	then
		echo "${filename[$i]} successfully completed!"
	else
		echo "${filename[$i]} failed!"
		good_tests=false;
	fi
done
echo $newline

if [ $good_tests = false ]
then
	echo "Some tests failed!"
else
	echo "All tests successfuly completed!"
fi
