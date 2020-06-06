output: iamroot.o TCP.o UDP.o geral.o interface.o
	gcc iamroot.o TCP.o UDP.o geral.o interface.o -lm -o iamroot

iamroot.o: iamroot.c functions.h geral.h
	gcc -g -c iamroot.c

TCP.o: TCP.c TCP.h geral.h
	gcc -g -c TCP.c

UDP.o: UDP.c UDP.h geral.h
	gcc -g -c UDP.c

geral.o: geral.c geral.h
	gcc -g -c geral.c

interface.o: interface.c interface.h geral.h
	gcc -g -c interface.c

clean: 
	rm *.o iamroot