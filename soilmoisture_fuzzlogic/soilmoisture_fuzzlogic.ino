
#define triggerPin  D1
#define echoPin     D2
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <Servo.h>
const char* serverName = "http://192.168.100.228/saskia/InsertDB.php";
const char* ssid = "ASTAGFIRULLOH";
const char* password = "1a2b3c4d5e6f";
// LiquidCrystal_I2C lcd(0x27, 16, 2);
// byte led = 16;
Servo myservo1, myservo2; // create servo object to control a servo
const int sensor_pin = A0;  /* Connect Soil moisture analog sensor pin to A0 of NodeMCU */
float suhu_moisture;
float kering,normal,basah;
float waterLevel;
float rendah,sedang,tinggi;
int pos = 0;
int servopin1 = 12;
int servopin2 = 14;
unsigned long lastTime = 0;
unsigned long timerDelay = 4000;

WiFiServer server(80);
unsigned char Kering(){
  if (suhu_moisture <= 10.00){kering =1;}
  else if (suhu_moisture >=00.00 && suhu_moisture <=10.00){kering=(10-suhu_moisture)/10;}
  else if (suhu_moisture >= 10){kering=0;}
  // else if (suhu_moisture >= 10.00 && suhu_moisture <=15.00){dingin =(15-suhu_moisture)/5;}
  return kering;
}
unsigned char Normal(){
  if (suhu_moisture >= 10.00){normal =0;}
  else if (suhu_moisture >=10.00 && suhu_moisture <=20.00){normal=(suhu_moisture-10)/10;}
  else if (suhu_moisture >= 20.00 && suhu_moisture <=30.00){normal =(30-suhu_moisture/10);}
  else if (suhu_moisture >= 40.00){normal =0;}
  return normal;
}

unsigned char Basah(){
  if (suhu_moisture >= 40.00){basah =0;}
  else if (suhu_moisture >=40.00){basah=(suhu_moisture-40)/10;}
  // else if (suhu_moisture >= 35.00 && suhu_moisture <=40.00){lembab =(suhu_moisture-25/5);}
  else if (suhu_moisture >= 40.00){basah =1;}
  return basah;
}
//deklarasi ultrasonik
unsigned char Rendah() {
  if (waterLevel <= 5){rendah =1;}
  else if (waterLevel >=5 && waterLevel <=10){rendah=(5-waterLevel)/5;}
  else if (waterLevel >=10){rendah =0;}
  return rendah;
}
unsigned char Sedang () {
  if (waterLevel <=15) {sedang =0;}
  // else if (suhu_moisture >=10.00 && suhu_moisture <=35.00){basah=(suhu_moisture-30)/5;}
  else if (waterLevel >=5 && waterLevel <=10){sedang=(waterLevel-5)/5;}
  else if (waterLevel >=10 && waterLevel <=15){sedang=(10-waterLevel)/5;}
  else if (waterLevel >=15){sedang =0;}
  return sedang;
}
unsigned char Tinggi () {
  if (waterLevel >=15) {tinggi =0;}
  else if (waterLevel >=15 && waterLevel <=20) {tinggi=(waterLevel-15)/5;}
  else if (waterLevel >=20) {tinggi=1;}
  return tinggi;
}
void fuzzifikasi(){
  //Kelembapan
  Kering();
  Normal();
  Basah();
  //Ultrasonik
  Rendah();
  Sedang();
  Tinggi();
}

void setup() {
  
  Serial.begin(9600);
  myservo1.attach(servopin1);
  myservo2.attach(servopin2);
  myservo1.write(0);
  myservo2.write(0);
  // myservo1.write(0);
  // myservo2.write(0);
  Serial.println("Servo Terdeteksi");
  pinMode(triggerPin,OUTPUT);
  pinMode(echoPin, INPUT);  
  Serial.println("Ultrasonik Terdeteksi");
  Serial.println("Deteksi Moisture");
  Serial.println("Connecting");
  Serial.println("XAMPP start ?");
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println("WiFi connected");
  Serial.println("Server started");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  long duration, jarak;
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2); 
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  jarak = duration / 29.1;
  waterLevel = 36-jarak;
  suhu_moisture= ( 100.00 - ( (analogRead(sensor_pin)/1023.00) * 100.00 ) );
  delay(4000);
  fuzzifikasi();
  Serial.println("ketingian air"+ (String(waterLevel))); //nilai waterlevel
  Serial.println("kelembapan"+ (String(suhu_moisture))); //nilai waterlevel
  //KODINGAN BENER SERVO
        //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = (String)"moistur= suhu air = " + suhu_moisture +"%" + "&waterlevel= tinggi air =" + waterLevel +"cm";           
    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    http.end();
  }else {
        Serial.println("WiFi Disconnected");
    }
  if (suhu_moisture <= 10.00 && waterLevel <=5.00 ){
    
    Serial.print("Kering = "+ String(suhu_moisture)+"%");
    Serial.println(" "+String(kering));
    Serial.print("Rendah = "+ String(waterLevel)+" cm");
    Serial.println(" "+String(rendah));
    
    if (myservo1.read() == 0) {
      for(pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees
      { // in steps of 1 degree
      //membuka gerbang
        myservo1.write(180); // tell servo to go to position in variable 'pos'
              // myservo2.write(pos); // tell servo to go to position in variable 'pos'
        delay(15); // waits 15ms for the servo to reach the position
            // while (1);
        myservo1.detach();
      }  
    }
    myservo2.write(0);
  }
  else if (suhu_moisture >= 10.00 && suhu_moisture <=30 && waterLevel <= 5.00){

    myservo1.attach(12);
    Serial.print("Normal = "+ String(suhu_moisture)+"%");
    Serial.println(" "+String(normal));
    Serial.print("Rendah = "+ String(waterLevel)+" cm");
    Serial.println(" "+String(rendah));

    myservo1.write(140);

    if (myservo1.read() == 140) {
      for(pos = 140; pos >= 1; pos -= 1) // goes from 0 degrees to 180 degrees
      { // in steps of 1 degree
        myservo1.write(140); // tell servo to go to position in variable 'pos'
        delay(5); // waits 15ms for the servo to reach the position
        myservo1.detach();
      }
    }
    myservo2.write(0);
  }else if (suhu_moisture >= 20.00 && suhu_moisture <= 30.00 && waterLevel >=10.00 && waterLevel <=20.00 ) {
    myservo1.attach(12);
        
    Serial.print("Normal = "+ String(suhu_moisture)+"%");
    Serial.println(" "+String(normal));
    Serial.print("Sedang = "+ String(waterLevel)+" cm");
    Serial.println(" "+String(sedang));
    myservo1.write(100);
    if (myservo1.read() == 100) {
      for(pos = 100; pos >= 1; pos -= 1) // goes from 0 degrees to 180 degrees
      { // in steps of 1 degree
       myservo1.write(100); // tell servo to go to position in variable 'pos'
       delay(5); // waits 15ms for the servo to reach the position
       myservo1.detach();
      }
    }
    myservo2.write(0); 
  }else if (suhu_moisture >= 30.00 && suhu_moisture <= 40.00 && waterLevel <=5.00 ) {
    myservo1.attach(12);
          
    Serial.print("Basah = "+ String(suhu_moisture)+"%");
    Serial.println(" "+String(basah));
    Serial.print("Rendah = "+ String(waterLevel)+" cm");
    Serial.println(" "+String(rendah));
    myservo1.write(100);
    if (myservo1.read() == 100) {
      for(pos = 100; pos >= 1; pos -= 1) // goes from 0 degrees to 180 degrees
      { // in steps of 1 degree
        myservo1.write(100); // tell servo to go to position in variable 'pos'
        delay(5); // waits 15ms for the servo to reach the position
        myservo1.detach();
      }
    }
    myservo2.write(0);
  }else if (suhu_moisture >= 40.00 && waterLevel >= 20.00){
    
    
    Serial.print("Basah = "+ String(suhu_moisture)+"%");
    Serial.println(" "+String(basah));
    Serial.print("Tinggi = "+ String(waterLevel)+" cm");
    Serial.println(" "+String(tinggi)); 
    myservo1.attach(12);
    myservo1.write(90);
    myservo2.write(180);
    // myservo2.attach(14);
    
    if (myservo1.read() == 90) {
      for(pos = 90; pos >= 1; pos -= 1) // goes from 0 degrees to 180 degrees
      { // in steps of 1 degree
       myservo1.write(0); // tell servo to go to position in variable 'pos'
       delay(5); // waits 15ms for the servo to reach the position
       myservo1.detach();
      }
      myservo1.attach(12);
    }
    
  }
  
  lastTime = millis();
      //if paling atas bagian void loop
}