main: buildin
	gcc main.c datastructs.c strtools.c commands.c history.c debug.c -g -o shell.out

buildin:
	gcc ./build-in/help/help.c -o ./build-in/help/help.out -g
	gcc ./build-in/history/history.c -o ./build-in/history/history.out -g
