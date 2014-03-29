default: build

build:
	@echo Beginning build.
	@echo
	mkdir build
	cd src && make

test: build
	cd test && make

test-debug: build
	@echo Compiling tests with debug statements.
	@echo
	cd test && make DEBUG=1

clean:
	rm -rf ./build
	cd test && make clean
	@echo
	@echo Cleaned all compiled files.
