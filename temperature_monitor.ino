#include <OneWire.h>
#include <DallasTemperature.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SoftwareSerial.h>

// ---------- Pins ----------
const byte ONE_WIRE_PIN = 2;     // DS18B20 DAT
SoftwareSerial bt(10, 11);       // RX=D10 (HC-06 TXD), TX=D11 (divider -> RXD)

// ---------- Objects ----------
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);
U8G2_SSD1306_128X64_NONAME_1_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);  // page mode: 128 B buffer

// ---------- Timing ----------
const unsigned long CONV_MS = 750;   // DS18B20 12-bit max conversion time (datasheet)
unsigned long convStart = 0;

// ---------- Data ----------
float tempC = 0;
float minT = 1000, maxT = -1000;
bool sensorOK = false;
bool wasConnected = false;           // was the sensor answering on the last read?
unsigned long txCount = 0;

// ---------- Circular buffer (fixed size: old data is overwritten) ----------
const byte HIST_LEN = 64;            // 64 points
const byte HIST_EVERY = 3;           // store 1 point every 3 readings (~2.25 s)
int16_t hist[HIST_LEN];              // temp x100 as integer -> 2 B each = 128 B
byte histHead = 0;                   // next write position
byte histCount = 0;                  // how many valid points
byte readingCounter = 0;

void addToHistory(float t) {
  hist[histHead] = (int16_t)(t * 100);           // overwrite oldest
  histHead = (histHead + 1) % HIST_LEN;          // wrap around
  if (histCount < HIST_LEN) histCount++;
}

void startConversion() {
  sensors.requestTemperatures();     // returns immediately (non-blocking)
  convStart = millis();
}

void setup() {
  Serial.begin(115200);
  bt.begin(9600);
  sensors.begin();
  sensors.setResolution(12);
  sensors.setWaitForConversion(false);   // do NOT block the CPU for 750 ms
  oled.begin();
  startConversion();
}

void drawGraph() {
  const byte gx = 0, gy = 43, gw = 128, gh = 21;
  oled.drawFrame(gx, gy, gw, gh);
  if (histCount < 2) return;

  // Find range of stored points (auto-scale, min span 1.00 C)
  byte oldest = (histHead + HIST_LEN - histCount) % HIST_LEN;
  int16_t lo = 32767, hi = -32768;
  for (byte i = 0; i < histCount; i++) {
    int16_t v = hist[(oldest + i) % HIST_LEN];
    if (v < lo) lo = v;
    if (v > hi) hi = v;
  }
  if (hi - lo < 100) { int16_t mid = (hi + lo) / 2; lo = mid - 50; hi = mid + 50; }

  // Draw lines, newest point on the right
  byte prevX = 0, prevY = 0;
  for (byte i = 0; i < histCount; i++) {
    int16_t v = hist[(oldest + i) % HIST_LEN];
    byte x = gx + gw - 2 - (histCount - 1 - i) * 2;
    byte y = gy + gh - 2 - (long)(v - lo) * (gh - 4) / (hi - lo);
    if (i > 0) oled.drawLine(prevX, prevY, x, y);
    prevX = x; prevY = y;
  }
}

void drawScreen() {
  oled.firstPage();
  do {
    // Header bar
    oled.drawBox(0, 0, 128, 12);
    oled.setDrawColor(0);
    oled.setFont(u8g2_font_6x10_tr);
    oled.drawStr(3, 10, "TEMP MONITOR");
    oled.drawStr(84, 10, "DS18B20");
    oled.setDrawColor(1);

    if (sensorOK) {
      // Big temperature (left)
      char buf[8];
      dtostrf(tempC, 5, 2, buf);
      oled.setFont(u8g2_font_logisoso24_tn);
      oled.drawStr(0, 40, buf);
      oled.drawCircle(74, 19, 2);
      oled.setFont(u8g2_font_ncenB10_tr);
      oled.drawStr(78, 30, "C");

      // Right column: max / min / tx count
      oled.setFont(u8g2_font_5x8_tr);
      char line[12], a[7];
      dtostrf(maxT, 5, 2, a);
      snprintf(line, sizeof(line), "H %s", a);
      oled.drawStr(92, 21, line);
      dtostrf(minT, 5, 2, a);
      snprintf(line, sizeof(line), "L %s", a);
      oled.drawStr(92, 30, line);
      snprintf(line, sizeof(line), "TX %lu", txCount);
      oled.drawStr(92, 39, line);

      drawGraph();
    } else {
      oled.setFont(u8g2_font_ncenB10_tr);
      oled.drawStr(14, 36, "SENSOR ERROR");
      oled.setFont(u8g2_font_6x10_tr);
      oled.drawStr(10, 54, "Check DAT wire (D2)");
    }
  } while (oled.nextPage());
}

void loop() {
  // 1) Read sensor when conversion is done (CPU free meanwhile)
  if (millis() - convStart >= CONV_MS) {
    float t = sensors.getTempCByIndex(0);
    bool connected = (t != DEVICE_DISCONNECTED_C);
    // 85.00 C is the DS18B20 power-on value. Right after power-up or after
    // re-plugging the probe it is not a real measurement, so skip it once.
    bool powerOnValue = connected && !wasConnected && t == 85.0;
    wasConnected = connected;
    if (powerOnValue) {
      startConversion();
      return;
    }
    sensorOK = connected;

    if (sensorOK) {
      tempC = t;
      if (t < minT) minT = t;
      if (t > maxT) maxT = t;
      if (++readingCounter >= HIST_EVERY) {
        readingCounter = 0;
        addToHistory(t);
      }
      bt.print("T=");  bt.print(t, 2);  bt.println("C");
      Serial.print("T="); Serial.print(t, 2); Serial.println("C");
    } else {
      bt.println("T=ERR");
      Serial.println("T=ERR");
    }
    txCount++;

    drawScreen();
    startConversion();
  }

  // 2) Commands from Bluetooth: 'R' resets min/max
  while (bt.available()) {
    char c = bt.read();
    if (c == 'R' || c == 'r') {
      minT = 1000; maxT = -1000;
      bt.println("MIN/MAX RESET");
    }
  }
}
