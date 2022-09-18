#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "../includes/conversions.h"
#include "../includes/cpu.h"
#include "../includes/bus.h"
//#include "doctest.h"

void loadTestRam() {
  Cpu cpu = Cpu();
  cpu.bus.writeRam8(0x600, 0xa9); // LDA #$10
  cpu.bus.writeRam8(0x601, 0x69);
  cpu.bus.writeRam8(0x602, 0x8d); // STA $6000
  cpu.bus.writeRam16(0x603, 0x6000);
  cpu.bus.writeRam8(0x605, 0xa9); // LDA #$0
  cpu.bus.writeRam8(0x606, 0);
  cpu.bus.writeRam8(0x607, 0xad); // LDA $6000
  cpu.bus.writeRam16(0x608, 0x6000);
  cpu.bus.writeRam8(0x60a, 0);
  cpu.bus.writeRam16(0x6000, 0x6000);
  cpu.bus.writeRam16(0xfffc, 0x600);
  cpu.pc = cpu.bus.readRam16(0xfffc);
  while (cpu.pc < 0x60a) {
    cpu.executeIns();
    printf("pc: 0x%.4x\n", cpu.pc);
    printf("sp: 0x%.2x\n", cpu.sp);
    printf("A: 0x%.2x\n", cpu.A);
    printf("X: 0x%.2x, Y: 0x%.2x \n", cpu.X, cpu.Y);
    //uint8_t bit = 1;
    printf("   NVUBDIZC\n");
    printf("S: ");
    for (uint8_t i = Cpu::N; i != 0; i>>=1) {
      printf("%d", cpu.isFlagSet(i));
    }
  }
}
void loadTestFile(char * filename) {
  Cpu cpu = Cpu(filename);
  int timesRan = 0;
  //int limit = 50;
  //cpu.pc = bus.readRam16(0xfffc);
  while (!cpu.isFlagSet(Cpu::B)) {
    /*if (timesRan > limit) {
      printf("Limit Passed!\n");
      break;
    }*/
    cpu.executeIns();
    printf("pc: 0x%.4x\n", cpu.pc);
    printf("sp: 0x%.2x\n", cpu.sp);
    printf("A: 0x%.2x\n", cpu.A);
    printf("X: 0x%.2x, Y: 0x%.2x \n", cpu.X, cpu.Y);
    //uint8_t bit = 1;
    printf("   NVUBDIZC\n");
    printf("S: ");
    for (uint8_t i = Cpu::N; i != 0; i>>=1) {
      printf("%d", cpu.isFlagSet(i));
    }
    printf("\n");
    printf("cycles: %d\n", cpu.cyclecount);
    int delay = 2;
#ifdef _WIN32
    //Sleep(delay * 1000);
#else
    //sleep(delay);
#endif
timesRan++;
  }
  //if (timesRan <= limit) {
    printf("Break address Found!!  Ran successfully!\n");
    printf("pc: 0x%.4x\n", cpu.pc);
  //}
}
