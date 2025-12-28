kosmic: kosmic-cli kosmic-lib

kosmic-cli: src/*
	g++ src/kosmic_cli_application.cpp -std=c++11 -o kosmic-cli -pthread -O3 -I lib

kosmic-lib: src/*
	g++ src/kosmic_library.cpp -std=c++11 -DNDEBUG -fPIC -O3 -shared -o python-bindings/kosmic/kosmic-lib.so
