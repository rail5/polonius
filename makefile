all: reader editor
	
reader:
	cd read && $(MAKE)
	mv read/bin/polonius-reader ./

editor:
	cd edit && $(MAKE)
	mv edit/bin/polonius-editor ./

clean:
	rm -f ./polonius-reader
	cd read && $(MAKE) clean
	rm -f ./polonius-editor
	cd edit && $(MAKE) clean
