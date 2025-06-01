CXX=g++
CXXFLAGS=-O2 -s

all: bin/polonius-editor

bin:
	mkdir -p bin

bin/shared: bin
	mkdir -p bin/shared

bin/%.o: src/%.cpp bin
	$(CXX) $(CXXFLAGS) -c src/$*.cpp -o bin/$*.o

bin/polonius-editor: bin bin/file.o bin/instruction.o bin/main.o bin/shared/explode.o bin/shared/to_lower.o bin/shared/is_number.o bin/shared/parse_block_units.o bin/shared/process_special_chars.o
	$(CXX) $(CXXFLAGS) -o bin/polonius-editor \
	bin/shared/explode.o bin/shared/to_lower.o bin/shared/is_number.o bin/shared/parse_block_units.o bin/shared/process_special_chars.o \
	bin/file.o bin/instruction.o bin/main.o

bin/shared/%.o: bin/shared ../shared/%.cpp
	$(CXX) $(CXXFLAGS) -c ../shared/$*.cpp -o bin/shared/$*.o

clean: bin bin/shared
	rm -f bin/*.o bin/polonius-editor
	rm -f bin/shared/*

.PHONY: clean
