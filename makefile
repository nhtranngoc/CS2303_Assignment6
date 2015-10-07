cptest: cptest.o
	gcc -g cptest.o -o cptest

cptest.o: cptest.c
	gcc -c -g cptest.c

clean:
	rm -f *.o cptest

docs:
	doxygen
	chmod a+r html/*
	cp -p html/* ~/public_html/cs2303assig6


