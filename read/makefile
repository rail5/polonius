CXX=g++
PARAMS=-O2 -s
MAYBE_SPLIT_STACK=-fsplit-stack

all: clean check build

check:
	# Check if we can use -fsplit-stack
	$(eval MAYBE_SPLIT_STACK=$(shell rm -rf checks; \
	mkdir checks; \
	echo "int main() { return 0; }" > checks/trivial_program.cpp; \
	$(CXX) -o checks/trivial_program -fsplit-stack checks/trivial_program.cpp \
	&& echo "-fsplit-stack" \
	|| echo "-Dno_split_stack"))
	rm -rf checks

build:
	$(CXX) -std=gnu++20 -o bin/polonius-reader $(PARAMS) $(MAYBE_SPLIT_STACK) src/main.cpp

clean:
	rm -f bin/polonius-reader
	rm -rf checks
