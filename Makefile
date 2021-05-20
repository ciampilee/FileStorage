Server: Server.c libprojectutils.a
	gcc Server.c -o Server projectutils/libprojectutils.a -pthread

libprojectutils.a:
	$(MAKE) -C projectutils