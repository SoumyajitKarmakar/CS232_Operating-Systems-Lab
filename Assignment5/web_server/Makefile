all: webserver

webserver_pool: webserver_pool.o tcp.o request.o
        gcc webserver_pool.o tcp.o request.o -o webserver_pool -g -lpthread

webserver_pool.o: webserver_pool.c
        gcc -Wall -g -c webserver_pool.c -o webserver_pool.o

webserver_dynamic: webserver_dynamic.o tcp.o request.o
        gcc webserver_dynamic.o tcp.o request.o -o webserver_dynamic -g -lpthread

webserver_dynamic.o: webserver_dynamic.c
        gcc -Wall -g -c webserver_dynamic.c -o webserver_dynamic.o

webserver: webserver.o tcp.o request.o
        gcc webserver.o tcp.o request.o -o webserver -g -lpthread

webserver.o: webserver.c
        gcc -Wall -g -c webserver.c -o webserver.o

tcp.o: tcp.c tcp.h
        gcc -Wall -g -c tcp.c -o tcp.o

request.o: request.c request.h
        gcc -Wall -g -c request.c -o request.o

clean_pool:
        rm -f webserver_pool.o webserver_pool

clean_dynamic:
        rm -f webserver_dynamic.o webserver_dynamic

clean:
        rm -f webserver.o webserver