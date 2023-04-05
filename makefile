all: reader editor curses
	
reader:
	cd read && $(MAKE)
	mv read/bin/polonius-reader ./

editor:
	cd edit && $(MAKE)
	mv edit/bin/polonius-editor ./

curses:
	cd cli && $(MAKE)
	mv cli/bin/polonius ./

install:
	install -m 0755 polonius-reader /usr/bin
	install -m 0755 polonius-editor /usr/bin

clean:
	rm -f ./polonius-reader
	cd read && $(MAKE) clean
	rm -f ./polonius-editor
	cd edit && $(MAKE) clean
	rm -f ./polonius
	cd cli && $(MAKE) clean
