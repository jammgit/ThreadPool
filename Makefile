myecho : Echo.c
	g++ -std=c++11 -o MyEcho Echo.c -lpthread -I/usr/include/c++/5/


.PHONY clean:
clean:
	-rm myecho
