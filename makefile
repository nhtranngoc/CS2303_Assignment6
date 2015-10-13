all: cptest cptest_bonus

cptest: cptest.o
	gcc -g cptest.o -o cptest

cptest.o: cptest.c
	gcc -c -g cptest.c

cptest_bonus: cptest_bonus.o
	gcc -g cptest_bonus.o -o cptest_bonus

cptest_bonus.o: cptest_bonus.c
	gcc -c -g cptest_bonus.c

clean:
	rm -f *.o cptest
	rm -f cptest_bonus

docs:
	doxygen
	chmod a+r html/*
	cp -p html/* ~/public_html/cs2303assig6
