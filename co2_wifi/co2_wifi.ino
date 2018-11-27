/*
 *  This sketch sends data via HTTP POST to openhab2.
 *
 *  add your settings.h to set
 *  
 *  const char* ssid     = "";
 *  const char* password = "";
 *  const char* sensor_address = ""; // Openhab2 rest url
 */

#include <WiFi.h>
#include <HTTPClient.h>

#include "settings.h"

HTTPClient http;

void setup()
{
    Serial.begin(115200);
    delay(2000);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    
  Serial2.begin(9600);

  delay(1000);
  // Stop Sensor
  Serial2.write('K');
  Serial2.write(' ');
  Serial2.write('0');
  Serial2.write('\r');
  Serial2.write('\n' );

  delay(1000);
  

//  Serial.print("connecting to ");
//  Serial.println(host);
  
//  // Use WiFiClient class to create TCP connections
//  WiFiClient client;
//  const int httpPort = 8080;
//  if (!client.connect(host, httpPort)) {
//      Serial.println("connection failed");
//      return;
//  }
}

void loop()
{
  int incomingByte = 0;   // for incoming serial data
  int co2 = 0;  
  
  Serial2.write('K');
  Serial2.write(' ');
  Serial2.write('2');
  Serial2.write('\r');
  Serial2.write('\n' );

  delay(5000);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  
  Serial.println("WiFi connected");
  
  Serial2.write('Z');
  Serial2.write('\r');
  Serial2.write('\n' );
  
  while (Serial2.available() > 0) {
        // read the incoming byte:
        incomingByte = Serial2.read();
        Serial.write(incomingByte);
        if (incomingByte == 'Z')
        {
          incomingByte = Serial2.read(); // ' '
          Serial.write(incomingByte);
  
          co2 = 0;
  
          incomingByte = Serial2.read();
          Serial.write(incomingByte);
          co2 += (incomingByte - 0x30) * 10000;
          
          incomingByte = Serial2.read();
          Serial.write(incomingByte);
          co2 += (incomingByte - 0x30) * 1000;
          
          incomingByte = Serial2.read();
          Serial.write(incomingByte);
          co2 += (incomingByte - 0x30) * 100;
         
          incomingByte = Serial2.read();
          Serial.write(incomingByte);
          co2 += (incomingByte - 0x30) * 10;
          
          incomingByte = Serial2.read();
          Serial.write(incomingByte);
          co2 += incomingByte - 0x30;
  
          Serial.println();
  
          Serial.print("New CO2 Value: ");
          Serial.println(co2);

          incomingByte = Serial2.read();

          if (incomingByte == 0x0A)
          {
            while (Serial2.available() > 0)
            {
              incomingByte = Serial2.read();
              Serial.write(incomingByte);
            }
            
            break;
          }
        }
  }

  // Put sensor to sleep
  Serial2.write('K');
  Serial2.write(' ');
  Serial2.write('0');
  Serial2.write('\r');
  Serial2.write('\n' );

  Serial.print("Sending Value: ");  
  Serial.println(String(co2));  
  
  http.begin(sensor_address); 
  http.addHeader("Content-Type", "text/plain"); //Specify content-type header
  int httpResponseCode = http.POST(String(co2)); //Send the actual POST request
  
  if(httpResponseCode > 0) {
      String response = http.getString();  //Get the response to the request
 
      Serial.print("return code: ");
      Serial.println(httpResponseCode);   //Print return code
      Serial.print("response: ");
      Serial.println(response);           //Print request answer
  } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
   
  }
  
  WiFi.disconnect();
  delay(25000);
}
