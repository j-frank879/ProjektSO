all:mutex_most var_most

mutex_most:mutex_most.c
	gcc -o mutex_most -pthread mutex_most.c
var_most:var_most.c
	gcc -o var_most -pthread var_most.c

