all:
	rm -fr build_release build_debug && mkdir build_release build_debug
	(cd build_debug   &&  cmake -DCMAKE_BUILD_TYPE=Debug   ../src)
	(cd build_release &&  cmake -DCMAKE_BUILD_TYPE=Release ../src)
	make VERBOSE=1 -C build_debug
	make VERBOSE=1 -C build_release
	cp build_release/js .

clean:
	rm -fr  build_release build_debug js
