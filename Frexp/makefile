CC = gcc
CFLAGS = -g -Wall

myfrexp: frexp.o
	$(CC) $(CFLAGS) -o myfrexp frexp.o

# Make already knows that you can generate .o file from its corresponding .cpp file.
# So only need to list the .h dependencies.

frexp.o: 
	$(CC) $(CFLAGS) -c frexp.c

clean:
	rm myfrexp *.o *~
