//#include "conversions.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

uint16_t Etoe(uint16_t n) {
  uint16_t mob = (n & 0xff00) >> 8;
  uint16_t lob = (n & 0xff) << 8;
  return lob | mob;
}
