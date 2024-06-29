all: main.exe

main.exe: main.o
	gcc -o main.exe main.o -lpdcurses

main.o: main.c 
	gcc -c --std=c99 -o main.o main.c -lpdcurses

headers.o: headers.c 
	gcc -c --std=c99 -o headers.o headers.c -lpdcurses

interface.o: interface.c 
	gcc -c --std=c99 -o interface.o interface.c -lpdcurses

structs.o: structs.c 
	gcc -c --std=c99 -o structs.o structs.c -lpdcurses

enums.o: enums.c 
	gcc -c --std=c99 -o enums.o enums.c -lpdcurses

clean:
	del *.o
	del main.exe
