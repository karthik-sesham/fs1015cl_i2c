#include <fs1015.h>

fs1015 fs = fs1015(true);

void setup() {
  Serial.begin(9600);  // start serial for output
  Serial.println("FS1015CL test!");
  
  if (! fs.begin()) {
    Serial.println("Couldnt start");
    while(1);
  }
  Serial.println("FS1015 found!");
  
  Serial.println(fs.read_addr());
  Serial.println(fs.read_filter_depth());
  Serial.println(fs.read_sn().c_str());
  fs.write_response_time(65);
  Serial.println(fs.read_response_time());
}

void loop() {
  Serial.println(fs.read_flowrate());
  Serial.println();
  delay(1000);

}
