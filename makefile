# Default to using all available CPU cores for parallel builds
# unless the user specifies a different number of jobs with -jN
ifeq ($(filter -j%,$(MAKEFLAGS)),)
MAKEFLAGS += -j$(shell nproc)
endif

WIKIDIRECTORY=polonius.wiki
WIKIUPSTREAM=https://github.com/rail5/polonius.wiki.git
VERSION=$$(dpkg-parsechangelog -l debian/changelog --show-field version)

FTXUI_LIBS = $(shell pkg-config --libs ftxui)

CXX = g++
CXXFLAGS = -O2 -s -std=gnu++20 -lboost_regex $(FTXUI_LIBS)

all: src/shared/version.h
	$(MAKE) bin/polonius-editor bin/polonius-reader

bin/%: bin/obj/%/main.o bin/obj/file.o bin/obj/polonius-editor/instruction.o bin/obj/shared/explode.o bin/obj/shared/to_lower.o bin/obj/shared/is_number.o bin/obj/shared/parse_block_units.o bin/obj/shared/process_special_chars.o bin/obj/shared/parse_regex.o
	$(CXX) -o $@ $^ $(CXXFLAGS)

bin/obj/%.o: src/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

bin/obj/polonius-editor/%.o: src/edit/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

bin/obj/polonius-reader/%.o: src/read/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

bin/obj/polonius/%.o: src/cli/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

bin/obj/shared/%.o: src/shared/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

src/shared/version.h: debian/changelog
	@# Read the latest version number from debian/changelog
	@# And update shared/version.h with that number
	@# This ensures that the output of --version
	@# For each of the binaries is always up-to-date
	@ \
	if [ "$(VERSION)" != "" ]; then \
		echo "#define program_version \"$(VERSION)\"" > src/shared/version.h; \
		echo "$(VERSION)"; \
	else \
		echo "#define program_version \"unknown\"" > src/shared/version.h; \
		echo "Could not parse debian/changelog for version number"; \
	fi

manual:
	@# Git pull wiki & run pandoc to create manual pages
	@# You must have Git and Pandoc installed for this
	@ \
	if [ -d "$(WIKIDIRECTORY)" ]; then \
		cd "$(WIKIDIRECTORY)" && git pull "$(WIKIUPSTREAM)"; \
	else \
		git clone "$(WIKIUPSTREAM)" "$(WIKIDIRECTORY)"; \
	fi;
	pandoc --standalone --to man "$(WIKIDIRECTORY)/Polonius-Editor.md" -o debian/polonius-editor.1
	pandoc --standalone --to man "$(WIKIDIRECTORY)/Polonius-Reader.md" -o debian/polonius-reader.1

test:
	@if [ ! -f bin/polonius-editor ] || [ ! -f bin/polonius-reader ]; then \
		echo "Binaries not found. Please run 'make all' first."; \
		exit 1; \
	fi
	@cd tests && ./run-tests

clean:
	@rm -f bin/obj/*.o
	@rm -f bin/obj/polonius-editor/*.o
	@rm -f bin/obj/polonius-reader/*.0
	@rm -f bin/obj/shared/*.o
	@rm -f bin/polonius-editor
	@rm -f bin/polonius-reader
	@rm -f src/shared/version.h
	@rm -f debian/polonius-editor.1
	@rm -f debian/polonius-reader.1
	@rm -f tests/debug/*
	@rm -f tests/results/*
	@echo "Cleaned up build artifacts."
	@if [ -d "$(WIKIDIRECTORY)" ]; then \
		rm -rf "$(WIKIDIRECTORY)"; \
		echo "Removed wiki directory $(WIKIDIRECTORY)"; \
	fi

.PHONY: all clean manual test
