WIKIDIRECTORY=polonius.wiki
WIKIUPSTREAM=https://github.com/rail5/polonius.wiki.git
VERSION=$$(dpkg-parsechangelog -l debian/changelog --show-field version)

CXX = g++
CXXFLAGS = -O2 -s -std=gnu++17

all: src/shared/version.h
	$(MAKE) bin/polonius-editor

bin/polonius-editor: bin/obj/edit/main.o bin/obj/edit/file.o bin/obj/edit/instruction.o bin/obj/shared/explode.o bin/obj/shared/to_lower.o bin/obj/shared/is_number.o bin/obj/shared/parse_block_units.o bin/obj/shared/process_special_chars.o
	$(CXX) $(CXXFLAGS) -o $@ $^

bin/obj/edit/%.o: src/edit/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin/obj/shared/%.o: src/shared/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/shared/version.h: debian/changelog
	@# Read the latest version number from debian/changelog
	@# And update shared/version.h with that number
	@# This ensures that the output of --version
	@# For each of the binaries is always up-to-date
	@ \
	if [ "$(VERSION)" != "" ]; then \
		echo "#define program_version \"$(VERSION)\"" > shared/version.h; \
		echo "$(VERSION)"; \
	else \
		echo "#define program_version \"unknown\"" > shared/version.h; \
		echo "Could not parse debian/changelog for version number"; \
	fi

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

clean:
	rm -f bin/obj/edit/*.o
	rm -f bin/obj/shared/*.o
	rm -f bin/polonius-editor
	rm -f src/shared/version.h
	rm -f debian/polonius-editor.1
	rm -f debian/polonius-reader.1

.PHONY: all clean manual
