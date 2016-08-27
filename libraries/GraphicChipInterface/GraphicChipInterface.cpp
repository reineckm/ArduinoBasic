#include <GraphicChipInterface.h>
#include <stdio.h>
#if not defined(__AVR_ATmega328P__)
#else
#include "Arduino.h"
#endif

void GraphicChipInterface::wait() {
  while (buff[0] != '9') {
    delayMicroseconds(1);
  }
  buff[0] = '0';
}

void GraphicChipInterface::clear() {
  wait();
  buff[0] = '0';
  buff[1] = '\a';
}

void GraphicChipInterface::print(char* s) {
  wait();
  sprintf(buff, "1%s\a", s);
}

void GraphicChipInterface::println(char* s) {
  wait();
  sprintf(buff, "2%s\a", s);
}

void GraphicChipInterface::println() {
  wait();
  buff[0] = '2';
  buff[1] = '\a';
}

void GraphicChipInterface::draw_row(int a, int b, int c, int d) {
  wait();
  sprintf(buff, "3%3d%3d%3d%1d\a", a, b, c, d);
}

void GraphicChipInterface::set_cursor(int a, int b) {
  wait();
  sprintf(buff, "4%2d%2d\a", a, b);
}

