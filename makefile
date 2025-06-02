# Default to using all available CPU cores for parallel builds
# unless the user specifies a different number of jobs with -jN
ifeq ($(filter -j%,$(MAKEFLAGS)),)
MAKEFLAGS += -j$(shell nproc)
endif

WIKIDIRECTORY=polonius.wiki
WIKIUPSTREAM=https://github.com/rail5/polonius.wiki.git
VERSION=$(shell dpkg-parsechangelog -l debian/changelog --show-field version)

FTXUI_LIBS = $(shell pkg-config --libs ftxui 2>/dev/null)

CXX = g++
CXXFLAGS = -O2 -std=gnu++20 -Wall -Wextra
LDFLAGS = -s -lboost_regex $(FTXUI_LIBS)

# Install directories
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin

SHARED_OBJS = \
	bin/obj/shared/explode.o \
	bin/obj/shared/to_lower.o \
	bin/obj/shared/is_number.o \
	bin/obj/shared/parse_block_units.o \
	bin/obj/shared/process_special_chars.o \
	bin/obj/shared/parse_regex.o \
	bin/obj/file.o \
	bin/obj/polonius-editor/instruction.o

all: src/shared/version.h
	$(MAKE) bin/polonius-editor bin/polonius-reader

bin/%: bin/obj/%/main.o $(SHARED_OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

bin/obj/%.o: src/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LDFLAGS)

bin/obj/polonius-editor/%.o: src/edit/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LDFLAGS)

bin/obj/polonius-reader/%.o: src/read/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LDFLAGS)

bin/obj/polonius/%.o: src/tui/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LDFLAGS)

bin/obj/shared/%.o: src/shared/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LDFLAGS)

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

clean: clean-manual clean-binaries clean-tests clean-wiki
	@rm -f bin/obj/*.o
	@rm -f bin/obj/polonius-editor/*.o
	@rm -f bin/obj/polonius-reader/*.o
	@rm -f bin/obj/shared/*.o
	@rm -f src/shared/version.h
	@echo "Cleaned up build artifacts."

clean-manual:
	@rm -f debian/polonius-editor.1
	@rm -f debian/polonius-reader.1
	@echo "Cleaned up manual pages."

clean-binaries:
	@rm -f bin/polonius-editor
	@rm -f bin/polonius-reader
	@echo "Cleaned up binaries."

clean-tests:
	@rm -f tests/debug/*
	@rm -f tests/results/*
	@echo "Cleaned up test results."

clean-wiki:
	@if [ -d "$(WIKIDIRECTORY)" ]; then \
		rm -rf "$(WIKIDIRECTORY)"; \
		echo "Removed wiki directory $(WIKIDIRECTORY)"; \
	fi

install: all
	@install -Dm755 bin/polonius-editor "$(BINDIR)/polonius-editor"
	@install -Dm755 bin/polonius-reader "$(BINDIR)/polonius-reader"
	@echo "Installed polonius binaries to $(BINDIR)"

uninstall:
	@rm -f "$(BINDIR)/polonius-editor"
	@rm -f "$(BINDIR)/polonius-reader"
	@echo "Uninstalled polonius binaries from $(BINDIR)"

.PHONY: all clean clean-manual clean-binaries clean-tests clean-wiki manual test install uninstall
