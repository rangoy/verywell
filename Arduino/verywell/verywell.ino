
// Cconfiguration pressure sensor
#define PRESSURE_PIN A5

#define VOLTAGE_MIN       0.4
#define VOLTAGE_MAX       4.4
#define PRESSURE_MIN_PSI  0
#define PRESSURE_MAX_PSI  30
#define PSI_TO_BAR        0.0689475729
#define PSI_TO_CM       70.306958


#define TRIGGER_PIN  6  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     7  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 600 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define FILTER_SIZE 33


#define SEND_INTERVAL 10000
#define SENS_INTERVAL 300
// ------------- dependencies for sonar
#include <NewPing.h>




// configuration for sonar

#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include <PubSubClient.h>
#include "SoftwareSerial.h"
#include <Filter.h>

//Copy and adapt LocalConfig.sample.h
#include "LocalConfig.h"




char thingsboardServer[] = THINGSBOARD_SERVER;

// Initialize the Ethernet client object
WiFiEspClient espClient;


PubSubClient client(espClient);

SoftwareSerial soft(2, 3); // RX, TX

int status = WL_IDLE_STATUS;
unsigned long lastSend;

//setup sonar
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

//Smohting filter
//#include <MedianFilter.h>
//MedianFilter sonarFilter(FILTER_SIZE,0);
//MedianFilter pressureFilter(FILTER_SIZE,0);
unsigned int rejectedSonarSamples = 0;
unsigned int samples = 0;

ExponentialFilter<float> sonarFilter(5, 0);
ExponentialFilter<float> pressureFilter(5, 0);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 
  Serial.println("INIT@115200");
  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  lastSend = 0;

}

void loop() {

  //logPressure();
  //logSonar();
  Serial.println(".");
  delay(SENS_INTERVAL);


  status = WiFi.status();
  Serial.println(status);
  if ( !( status == WL_CONNECTED  || status == WL_IDLE_STATUS) ) {
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(WIFI_AP);
      // Connect to WPA/WPA2 network
      status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      delay(500);
    }
    Serial.println("Connected to AP");
  }

  if ( !client.connected() ) {
    reconnect();
  }

  if (getAndPublishSensors( millis() - lastSend > SEND_INTERVAL )) { // Update and send only after 1 seconds
    
    lastSend = millis();
  }

  client.loop();
  
}

boolean getAndPublishSensors(boolean publishEnabled){

    samples++;
    
    String payload = "{ ";
    
    float rawBarCM = getPressureCM();
    float rawSonarCM = getSonar();
    payload += "\"rawBarCM\":"; payload += rawBarCM; payload += ",";
    
    if(rawSonarCM != 0) {
       sonarFilter.Filter(rawSonarCM);
       payload += "\"rawSonarCM\":"; payload += rawSonarCM; payload += ",";
    }
    else  rejectedSonarSamples++;
    
    if(rawBarCM != 0) {
       pressureFilter.Filter(rawBarCM);
    }
    
    float barCM = pressureFilter.Current();
    float sonarCM =sonarFilter.Current();
    payload += "\"sonarCM\":"; payload += sonarCM; payload += ",";
    payload += "\"samples\":"; payload += samples; payload += ",";
    payload += "\"rejectedSonars\":"; payload += rejectedSonarSamples; payload += ",";
    payload += "\"barCM\":"; payload += barCM; // payload += ",";
    
  payload += "}";

  Serial.println(payload);
  //Serial.println(barCM);  

  if(publishEnabled) {
  char attributes[150];
  payload.toCharArray( attributes, 150 );
   Serial.println( "sending data");
    
    client.publish( MQTT_TOPIC, attributes );
    rejectedSonarSamples = 0;
    samples = 0;
  }
  else {

    Serial.println( "feeding filter");
  }
  
  return publishEnabled;
}
//sonar
void logSonar(){
  float sonarCm = getSonar();

  Serial.print("Ping: ");
  Serial.print( sonarCm); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  Serial.print("cm ");

}

float getSonar(){
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  Serial.print("Ping: ");
  Serial.print( uS *1.0 / US_ROUNDTRIP_CM); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  Serial.print("cm ");
  return  uS *1.0 / US_ROUNDTRIP_CM;
  
}

////////// Pressure sensor
float getPressureCM(){

    
  float voltage = 1.0 * getAnalogVoltage(PRESSURE_PIN);
  float psi = (voltage - VOLTAGE_MIN) / (VOLTAGE_MAX - VOLTAGE_MIN ) * PRESSURE_MAX_PSI ;
  float bar =  psi * PSI_TO_BAR;
  float waterCm = psi * PSI_TO_CM;


/*  XXXX
  Serial.print(" PSI = ");
  Serial.print(psi);
  Serial.print(" bars ");

  Serial.print(" bar = ");
  Serial.print(bar);
  Serial.print(" bars ");

  Serial.print(" CM = ");
  Serial.print(waterCm);
  Serial.print(" bars ");*/
  return waterCm;

  
}

float getAnalogVoltage(int _pin){
  int sensorVal= 0;
  float voltage =0;
  sensorVal=analogRead(_pin);
  
  voltage = (sensorVal*5.0)/1024.0;
  
  Serial.print(" Unfiltred voltage = ");
  Serial.println(voltage);
  
  
  voltage = (sensorVal*5.0)/1024.0;
  
  

  return voltage; 
}




//// WIFI

void InitWiFi()
{
  // initialize serial for ESP module
  soft.begin(9600);
  // initialize ESP module
  WiFi.init(&soft);
  // check for the presence of the shield
   while (WiFi.status() == WL_NO_SHIELD) { //XXX if
    Serial.println("WiFi shield not present");
    // don't continue
    //while (true); //XXX
  }


  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_AP);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    delay(500);
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("Arduino Uno Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
