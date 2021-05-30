all: myserver myclient libprojectutils.a 

myserver: myserver.c libprojectutils.a
	gcc myserver.c projectutils/libprojectutils.a -o myserver  -pthread -Wall

myclient: myclient.c libprojectutils.a
	gcc myclient.c projectutils/libprojectutils.a -o myclient -Wall

libprojectutils.a:
	$(MAKE) -C projectutils
