default: build

clean:
	rm -f *.o
	rm -f notes++

fresh:
	make clean
	make

build:
	g++ -w -c color.cpp
	g++ -c notes.cpp
	g++ -o notes++ main.cpp notes.o color.o

rebuild:
	make clean
	make build

run:
	./notes++

runtest:
	./notes++ -db ./nppdb_test.db

runfreshtest:
	make fresh
	make runtest

freshruntest:
	make runfreshtest

memtest: fresh
	valgrind ./notes++

runfresh:
	make fresh
	make run

install:
	cp ./notes++ /usr/local/bin/
	ln -s -f /usr/local/bin/notes++ /usr/local/bin/npp

uninstall:
	rm /usr/local/bin/notes++
	rm /usr/local/bin/npp

gentest:
	touch nppdb_test.db
	echo "NPP-DB-HEAD" > nppdb_test.db
	echo "V:2" >> nppdb_test.db
	echo "C:6" >> nppdb_test.db
	echo "BEGIN" >> nppdb_test.db
	echo "N:0:hello world" >> nppdb_test.db
	echo "I:1:goodbye world" >> nppdb_test.db
	echo "N:0:watwatwat" >> nppdb_test.db
	echo "I:0:whodat" >> nppdb_test.db
	echo "I:2:archived task" >> nppdb_test.db
	echo "N:1:completed task" >> nppdb_test.db
	echo "EOF" >> nppdb_test.db
	echo "Completed generating test database..."
