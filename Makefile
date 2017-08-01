all:
	cd src; qmake; make
	cd test; qmake; make

clean:
	cd src; qmake; make clean
	rm -f src/Makefile
	cd test; qmake; make clean
	rm -f test/Makefile
	rm -f lib/libCQCharts.a
	rm -f test/CQChartsTest
