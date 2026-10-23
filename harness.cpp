// Runs the real sketch on the PC with a scripted DS18B20.
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include "Arduino.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "U8g2lib.h"
#include "SoftwareSerial.h"
static unsigned long now_ms = 0;
unsigned long millis(){ return now_ms; }
void delay(unsigned long d){ now_ms += d; }
char* dtostrf(double v, signed char w, unsigned char p, char* b){ sprintf(b, "%*.*f", w, p, v); return b; }
std::string out;
Stream Serial;
int Stream::available(){return 0;} int Stream::read(){return -1;} void Stream::begin(long){}
void Stream::print(const char*s){out+=s;} void Stream::print(float f,int p){char b[32];sprintf(b,"%.*f",p,f);out+=b;}
void Stream::print(long){} void Stream::print(unsigned long){} void Stream::print(int){} void Stream::print(char){}
void Stream::println(const char*s){out+=s;out+="\n";} void Stream::println(float,int){} void Stream::println(int){} void Stream::println(long){} void Stream::println(unsigned long){}
SoftwareSerial::SoftwareSerial(int,int){}
OneWire::OneWire(uint8_t){}
std::vector<float> script; size_t idx=0;
DallasTemperature::DallasTemperature(OneWire*){} void DallasTemperature::begin(){} void DallasTemperature::setResolution(uint8_t){}
void DallasTemperature::setWaitForConversion(bool){} void DallasTemperature::requestTemperatures(){}
float DallasTemperature::getTempCByIndex(uint8_t){ return idx<script.size()?script[idx++]:script.back(); }
#include "sketch.cpp"
int main(int argc,char**argv){
  for(int i=1;i<argc;i++) script.push_back(atof(argv[i]));
  setup();
  for(size_t k=0;k<script.size();k++){ now_ms+=760; loop(); }
  printf("%s", out.c_str());
  printf("MIN=%.2f MAX=%.2f HIST=%d\n", minT, maxT, histCount);
}
