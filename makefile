WIKIDIRECTORY=polonius.wiki
WIKIUPSTREAM=https://github.com/rail5/polonius.wiki.git
VERSION=$$(dpkg-parsechangelog -l debian/changelog --show-field version)

all: update-version reader editor curses

update-version:
	# Read the latest version number from debian/changelog
	# And update shared/version.h with that number
	# This ensures that the output of --version
	# For each of the binaries is always up-to-date
	@ \
	if [ "$(VERSION)" != "" ]; then \
		echo "#define program_version \"$(VERSION)\"" > shared/version.h; \
		echo "$(VERSION)"; \
	fi;

manual:
	# Git pull wiki & run pandoc to create manual pages
	# You must have Git and Pandoc installed for this
	@ \
	if [ -d "$(WIKIDIRECTORY)" ]; then \
		cd "$(WIKIDIRECTORY)" && git pull "$(WIKIUPSTREAM)"; \
	else \
		git clone "$(WIKIUPSTREAM)" "$(WIKIDIRECTORY)"; \
	fi;
	pandoc --standalone --to man "$(WIKIDIRECTORY)/Polonius-Editor.md" -o debian/polonius-editor.1
	pandoc --standalone --to man "$(WIKIDIRECTORY)/Polonius-Reader.md" -o debian/polonius-reader.1

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
