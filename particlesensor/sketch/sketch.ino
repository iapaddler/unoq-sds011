#include <Arduino_RouterBridge.h>
#include <SDS011.h> 

// Libraries installed:
// MsgPack 0.4.2
// Arduino_RouterBr
// GUL_NovaFitness

// The module uses the namespace GuL
// UNO Q usually maps Serial1 to pins D0 and D1
GuL::SDS011 sds(Serial1);

void setup() {
  Serial1.begin(9600); // SDS011 always runs at 9600
  Bridge.begin();
  Monitor.begin(115200);
  delay(1000);
  
  while (!Monitor && millis() < 500) {}
  
  sds.setToPassiveReporting();
  //sds.setToActiveReporting();
  
  for (uint8_t i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    Monitor.println("Set some LEDs");
  }
}

int loop_count = 0;

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
  

  delay(1000);
}
