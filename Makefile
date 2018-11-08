help:
	@echo "Usage:"
	@echo "	make help"
	@echo "	make shared"
	@echo "	make static"
	@echo "	make doc"
	@echo "	make upgrade"
doc:
	rm -rf ./docs
	if hash doxygen 2>/dev/null; then doxygen; fi
external:
	git submodule update --init
build: 
	mkdir -p build
static: external build
	cd build && cmake .. -DBUILD_SHARED_LIBS=OFF 
	cmake --build ./build 
shared: external build
	mkdir -p build
	cd build && cmake .. -DBUILD_SHARED_LIBS=ON
	cmake --build ./build 
tests: 	external build
	cd build && cmake .. -DBUILD_TESTING=ON 
	cmake --build ./build 
clean:
	rm -r docs
	cd build && make clean
upgrade: 
	rm -f upgrade.sh
	wget http://github.lan/cpp/cmake-ci/raw/master/upgrade.sh
	bash upgrade.sh