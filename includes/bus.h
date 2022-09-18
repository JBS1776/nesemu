#ifndef BUS
#define BUS

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class Bus {
	public:
		Bus();
		Bus(char * filename);
		~Bus();
		void writeRam8(int i, uint8_t val);
		uint8_t readRam8(int i);
		void writeRam16(int i, uint16_t val);
		uint16_t readRam16(int i);
	private:
		uint8_t ram[0x10000];
};
#endif
