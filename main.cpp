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
#include "includes/bus.h"
#include "includes/cpu.h"
#include "includes/conversions.h"
#include "testing/testing.h"

using namespace std;

int main() {
    //printf("TEST CUST\n");
    //loadTestRam();
    //printf("TEST FILE\n");
    //loadTestFile("testing/test.txt");
    loadTestFile("testing/test2");
}
