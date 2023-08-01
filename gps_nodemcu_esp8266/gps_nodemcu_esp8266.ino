#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Sesuaikan SSID dan PASS WiFi 
#define ssid "ASTAGFIRULLOH"
#define password "1a2b3c4d5e6f"

// REST API Endpoint
// sesuaikan ENVIRONMENT NAME 
String RESTAPI  = "https://<ENVIRONMENT NAME>.outsystemscloud.com/IOTGPSTracker/rest/GPS/Post";
String Device_Id = "01500000A";

TinyGPSPlus gps;
SoftwareSerial ss(4, 5); // The serial connection to the GPS device
String latitude , longitude;
String Json_Data = "";


void setup()
{
  Serial.begin(9600);

  ss.begin(9600);
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());

}


void sendDatatoRESTAPI(String Device_Id, String latitude, String longitude)
{
    WiFiClient client;
    HTTPClient http;
    http.begin(client, RESTAPI);
    http.addHeader("Content-Type", "application/json");
    Json_Data = "{\"DeviceId\":\"" + Device_Id + 
                 ", \"Latitude\": " + latitude + 
                 ", \"Longitude\": " + longitude +  
                 "}";
    Serial.print (Json_Data);
    int httpCode = http.POST(Json_Data);
      
    // httpCode will be negative on error
    if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  
}


void loop()
{
  
  Serial.print("Serial Status :");
  Serial.println(ss.available() > 0);
  
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    latitude = String(gps.location.lat(), 6);
    longitude = String(gps.location.lng(), 6);
    
    Serial.print(latitude);
    Serial.print(", ");
    Serial.print(longitude);
    
    sendDatatoRESTAPI(Device_Id, latitude, longitude);
  }
  else
  {
      Serial.print(F("INVALID"));
  }
  Serial.println();
  
  delay(5000);
}