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
#include "../includes/bus.h"
#include "../includes/cpu.h"

void loadTestRam();
void loadTestFile(char * filename);
