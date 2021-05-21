Server: Server.c clientv0.2 libprojectutils.a
	gcc Server.c -o Server projectutils/libprojectutils.a -pthread
	
clientv0.2:
	gcc clientv0.2.c -o clientv0.2 projectutils/libprojectutils.a

libprojectutils.a:
	$(MAKE) -C projectutils
