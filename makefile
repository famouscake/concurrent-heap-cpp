gcc:
	g++ -pthread -Wall -std=c++11 -fdiagnostics-color src/*.cpp -o bin/run.out

clang:
	clang++ -pthread -Wall -Wc++11-extensions -std=c++11 -g ./src/*.cpp -o ./bin/run.out

analyze:
	clang++ --analyze -Xanalyzer -analyzer-output=text -Wc++11-extensions -std=c++14 ./src/*.cpp

format:
	clang-format ./src/*.cpp ./src/*.h -i
