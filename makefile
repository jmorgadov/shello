main: buildin
	gcc main.c datastructs.c strtools.c commands.c history.c debug.c -g -o shell.out

buildin:
	gcc ./build-in/help/help.c -o ./build-in/help/help.out -g