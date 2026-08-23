#include <Arduino_RouterBridge.h>
#include "Arduino_LED_Matrix.h"
#include <SDS011.h> 
#include "fonts.h"

// Libraries installed:
// MsgPack 0.4.2
// Arduino_RouterBr
// GUL_NovaFitness

// The module uses the namespace GuL
// UNO Q usually maps Serial1 to pins D0 and D1
GuL::SDS011 sds(Serial1);
ArduinoLEDMatrix matrix;
uint8_t frame[104] = { 0 };  // 8 rows x 13 columns, frame[row * 13 + col]

const int MATRIX_COLS = 13;
const int MATRIX_ROWS = 8;
const int MAX_MEAS 99;

// Two 5-px-wide digits (5 + 5 = 10 cols) centered in 13 columns,
// leaving a 1-column gap between them: positions 0 and 6.
const int TENS_POS = 0;
const int ONES_POS = 6;

int meas_count = 0;
int led = 0;
int red = 0;
int green = 1;
int blue = 0;

void clear_frame() {
  memset(frame, 0, sizeof(frame));
}

void display_frame() {
  matrix.draw(frame);
}

// Stamps a single digit's 5x8 bitmap into frame at column `pos`.
void add_digit_to_frame(int digit, int pos) {
  if (digit < 0 || digit > 9) return;
  for (int row = 0; row < MATRIX_ROWS; row++) {
    uint8_t bits = fonts[digit][row];
    for (int col = 0; col < 5; col++) {
      if (pos + col < MATRIX_COLS) {
        frame[row * MATRIX_COLS + pos + col] |= (bits >> (4 - col)) & 1;
      }
    }
  }
}

// Displays `value` (0-99) as two static digits, no scrolling.
void show_number(int value) {
  value = constrain(value, 0, 99);
  int tens = value / 10;
  int ones = value % 10;

  clear_frame();
  add_digit_to_frame(tens, TENS_POS);
  add_digit_to_frame(ones, ONES_POS);
  display_frame();
}

void setup() {
  Serial1.begin(9600); // SDS011 always runs at 9600
  Bridge.begin();
  Monitor.begin(115200);
  matrix.begin();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED4_R, OUTPUT);
  pinMode(LED4_G, OUTPUT);
  pinMode(LED4_B, OUTPUT);
  delay(1000);
  
  while (!Monitor && millis() < 500) {}
  
  sds.setToPassiveReporting();
  //sds.setToActiveReporting();

  show_number(0);

for (uint8_t i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    Monitor.println("Set some LEDs");
  }
}

void loop() {
  float pm25, pm10;
  const char *sname;
  bool res;
  char buf[80];
  
  if (Serial1.available()) {
    Monitor.println("Serial1 available");
  }

  sname = sds.getSensorName();
  Monitor.print("Sensor name: ");
  Monitor.println(sname);

  res = sds.pollReportingMode();
  Monitor.print("Reporting Mode: ");
  Monitor.println(res);
  
  res = sds.poll();
  Monitor.print("Poll: ");
  Monitor.println(res);
  delay(200);
  
  res = sds.read();
  Monitor.print("Read: ");
  Monitor.println(res);

  pm25 = sds.getPM2_5();
  sprintf(buf, "PM2.5: %.2f", pm25);
  Monitor.println(buf);

  pm10 = sds.getPM10();
  sprintf(buf, "PM10: %.2f", pm10);
  Monitor.println(buf);

  Bridge.notify("record_pm_values", pm25, pm10);
  
  led = !led;
  red = !red;
  green = !green;
  blue = !blue;
  Monitor.print("LED: ");
  Monitor.println(led);
  digitalWrite(LED_BUILTIN, led);
  digitalWrite(LED4_R, red);
  digitalWrite(LED4_G, green);
  digitalWrite(LED4_B, blue);

  meas_count++;
  show_number(meas_count);
  if (meas_count >= MAX_MEAS) {
     meas_count = 0;
  }

  delay(300000UL); // Delay for 5 minutes (300,000 ms)
}
