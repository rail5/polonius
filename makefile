all: reader editor
	
reader:
	cd read && $(MAKE)
	mv read/bin/reader ./

editor:
	cd edit && $(MAKE)
	mv edit/bin/editor ./

clean:
	rm -f ./reader
	cd read && $(MAKE) clean
	rm -f ./editor
	cd edit && $(MAKE) clean
