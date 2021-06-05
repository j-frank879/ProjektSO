all:var_most

var_most:var_most.c
	gcc -o var_most -pthread var_most.c
