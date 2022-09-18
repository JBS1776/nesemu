main:
	g++ -std=c++11 -Wall testing/testing.cpp main.cpp cpu.cpp bus.cpp conversions.cpp -I /includes -o main
testing_main:
	g++ -std=c++11 -Wall testing/testing.cpp testing/testing_cases.cpp testing/testing_main.cpp cpu.cpp bus.cpp conversions.cpp -I /includes -o testing_main 
clean:
	rm main testing_main
