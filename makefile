all: update-version reader editor curses

update-version:
	# Read the latest version number from debian/changelog
	# And update shared/version.h with that number
	# This ensures that the output of the --version arg
	# For each of the binaries is always up-to-date
	# On each compilation
	VERSION=`grep -P -o -e "([0-9\.]*)" debian/changelog | head -n 1`; \
	echo "#define program_version \"$$VERSION\"" > shared/version.h

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
