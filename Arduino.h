#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string>
typedef uint8_t byte;
unsigned long millis();
void delay(unsigned long);
char* dtostrf(double, signed char, unsigned char, char*);
class Stream {
public:
  int available();
  int read();
  void begin(long);
  void print(const char*);
  void print(float, int = 2);
  void print(long);
  void print(unsigned long);
  void print(int);
  void print(char);
  void println(const char* = "");
  void println(float, int = 2);
  void println(int);
  void println(long);
  void println(unsigned long);
};
extern Stream Serial;
