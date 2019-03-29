/*
  Arduino Webserver using ESP8266
  Displays temperature in a webpage

  Arduino Mega has three Serial communication ports,this code works well with 
  Arduino Mega.For UNO users,use Softserial library to connect ESP8266 with
  Arduino UNO 
  If you're unsure about your Arduino model or its pin cofigurations,please check
  the documentation at http://www.arduino.cc
 
  modified August 2016
  By Joyson Chinta and Gladson Kujur
 */

#include<SoftwareSerial.h>

#define DEBUG true

SoftwareSerial esp8266(3,2);


#define CONNECT_TO_AP "AT+CWJAP=\"AP\",\"Password\""

void setup()
{
  Serial.begin(9600);    ///////For Serial monitor 
  Serial.print("READY");
  
  esp8266.begin(9600); ///////ESP Baud rate
  pinMode(11,OUTPUT);    /////used if connecting a LED to pin 11
  digitalWrite(11,LOW);
 
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  sendData("AT+CWMODE=1\r\n",1000,DEBUG); // configure as client
  sendData(CONNECT_TO_AP,20000,DEBUG);
  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
}


float sensetemp() ///////function to sense temperature.
 {
  int val = analogRead(A0);
  float mv = ( val/1024.0)*5000; 
  float celcius = mv/10;
  return(celcius);
 }
 
int connectionId;
void loop()
{
  if(esp8266.available())
  {
    Serial.println("loop esp ava");
    /////////////////////Recieving from web browser to toggle led
    if(esp8266.find("+IPD,"))
    {
     delay(300);
     connectionId = esp8266.read()-48;
     if(esp8266.find("pin="))
     { 
     Serial.println("recieving data from web browser");
     int pinNumber = (esp8266.read()-48)*10; 
     pinNumber += (esp8266.read()-48); 
     digitalWrite(pinNumber, !digitalRead(pinNumber));
     }
   
    /////////////////////Sending data to browser
    else
    {
      String webpage = "<h1>Hello World</h1>";
      espsend(webpage);
     }
    
     if(sensetemp() != 0)
     {
       String add1="<h4>Temperature=</h4>";
      String two =  String(sensetemp(), 3);
      add1+= two;
      add1+="&#x2103";   //////////Hex code for degree celcius
      espsend(add1);
     }
    
     else
     {
      String c="sensor is not conneted";
      espsend(c);                                     
     } 
     
     String closeCommand = "AT+CIPCLOSE=";  ////////////////close the socket connection////esp command 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     sendData(closeCommand,3000,DEBUG);
    }
  }
}
 
  //////////////////////////////sends data from ESP to webpage///////////////////////////
 
 void espsend(String d)
         {
             String cipSend = " AT+CIPSEND=";
             cipSend += connectionId; 
             cipSend += ",";
             cipSend +=d.length();
             cipSend +="\r\n";
             sendData(cipSend,1000,DEBUG);
             sendData(d,1000,DEBUG); 
         }

//////////////gets the data from esp and displays in serial monitor///////////////////////         
String sendData(String command, const int timeout, boolean debug)
            {
           
              if(debug)
                     {
                     Serial.print("Sending command: " ); 
                     Serial.println(command); 
                     }
                String response = "";
                esp8266.print(command);
                long int time = millis();
                while( (time+timeout) > millis())
                {
                   while(esp8266.available())
                      {
                         char c = esp8266.read(); // read the next character.
                         response+=c;
                      }  
                }
                
                if(debug)
                     {
                     Serial.print("Response: " ); 
                     Serial.println(response); //displays the esp response messages in arduino Serial monitor
                     }
                return response;
            }