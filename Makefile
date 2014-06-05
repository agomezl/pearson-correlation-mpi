build : bin/Run-mpi bin/Run-serial

bin/Run-mpi : src/mpi/Main.cpp src/mpi/Util.h bin/
	mpic++ $< -o $@

bin/Run-serial : src/serial/Main.cpp src/serial/Util.h bin/
	g++ $< -o $@

bin/ :
	mkdir -p bin/

run-serial : bin/Run-serial
	$< examples/users.matrix

clean : 
	rm -fr bin/
