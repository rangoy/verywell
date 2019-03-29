// ---------------------------------------------------------------------------
// Example NewPing library sketch that does a ping about 20 times per second.
// ---------------------------------------------------------------------------


#include <NewPing.h>
#include <MedianFilter.h>
//#include <Wire.h>
#include <MedianFilter.h>

#define TRIGGER_PIN  6  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     7  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 450 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.


NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

MedianFilter filter(31,0);

void setup() {
  
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
 
}

void loop() {
  
  delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int o,uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).

  filter.in(uS);
  o = filter.out();
  Serial.print("Ping: ");
  Serial.print( o *1.0 / US_ROUNDTRIP_CM*1.0); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  Serial.print("cm");

  Serial.print("Pingraw: ");
  Serial.print( uS *1.0 / US_ROUNDTRIP_CM*1.0); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  Serial.println("cm");

}
