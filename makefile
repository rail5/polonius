all: reader editor
	
reader:
	cd src/read && $(MAKE)
	mv src/read/bin/reader ./

editor:
	cd src/edit && $(MAKE)
	mv src/edit/bin/editor ./

clean:
	rm -f ./reader
	cd src/read && $(MAKE) clean
	rm -f ./editor
	cd src/edit && $(MAKE) clean
