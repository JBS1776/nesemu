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
#include "includes/bus.h"
#include "includes/conversions.h"

using namespace std;

Bus::Bus() {
    for (int i = 0; i < 0x10000; i++) {
        ram[i] = 0;
    }
}

//TODO: Account for improper file length or file not exists!!!
Bus::Bus(char * filename) {
  ifstream file;
  file.open(filename, ios_base::in | ios::binary);
  if (file.is_open()) {
    char c;
    uint16_t index = 0;
    while (file.get(c)) {
      ram[index++] = (uint8_t) c;
    }
  }
}

Bus::~Bus() {

}

void Bus::writeRam8(int i, uint8_t val) {
    ram[i & 0xffff] = val;
}

uint8_t Bus::readRam8(int i) {
    return ram[i & 0xffff];
}
void Bus::writeRam16(int i, uint16_t val) {
    //TODO: Make sure val != 0xffff!!!!
    ram[i++ & 0xffff] = (uint8_t)(val & 0xff);
    ram[i & 0xffff] = (uint8_t)((val & 0xff00) >> 8);
}
uint16_t Bus::readRam16(int i) {
    //TODO: Make sure i != 0xffff!!!!!
    return (uint16_t) ram[i++ & 0xffff] | ((uint16_t)(ram[i & 0xffff]) << 8);
}
