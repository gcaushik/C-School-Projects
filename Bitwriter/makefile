CC = gcc
CFLAGS = -g -Wall

bitwriter: bitwriter.o
	$(CC) $(CFLAGS) -o bitwriter bitwriter.o

# Make already knows that you can generate .o file from its corresponding .cpp file.
# So only need to list the .h dependencies.

bitwriter.o: 
	$(CC) $(CFLAGS) -c bitwriter.c

clean:
	rm bitwriter *.o *~
