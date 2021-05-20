libprojectutils.a : projectutils.o projectutils.h.gch
	ar rs libprojectutils.a projectutils.o

projectutils.o : projectutils.c
	gcc -c projectutils.c

projectutils.h.gch : projectutils.h
	gcc -c projectutils.h