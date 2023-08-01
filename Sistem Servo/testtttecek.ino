#include <Servo.h>

Servo myservo1;
Servo myservo2;
int pos1;
int pos2;
int soilMoistureValue;

const int soilMoisturePin = A0;
//int sensorValue = 0;
const int ultrasonicTrig = 5;
const int ultrasonicEcho = 4;
const int servoPin1 = 14;
const int servoPin2 = 12;


//Deklarasi variabel sensor kelembapan
float soilMoisture;
float kering,normal,basah;
unsigned char Kering() {
  if (soilMoisture <= 30.00){kering =1;}
  else if (soilMoisture >=30.00 && soilMoisture <=60.00){kering=(60-soilMoisture)/30;}
  else if (soilMoisture >= 60) {kering=0;}
  return kering;
}
unsigned char Normal() {
  if (soilMoisture <= 30.00){normal =0;}
  else if (soilMoisture >=30.00 && soilMoisture <=60.00){normal=(60-soilMoisture)/30;}
  else if (soilMoisture >=60.00 && soilMoisture <=90.00){normal=(90-soilMoisture)/30;}
  else if (soilMoisture >= 90) {normal=0;}
  return normal;
}
unsigned char Basah() {
  if (soilMoisture <= 60.00){basah =0;}
  else if (soilMoisture >=60.00 && soilMoisture <=90.00){basah=(90-soilMoisture)/60;}
  else if (soilMoisture >= 90) {basah=1;}
  return basah;
}
//Deklarasi variabel ketinggian air
float waterLevel;
float rendah,sedang,tinggi;
unsigned char Rendah() {
  if (waterLevel <= 20){rendah =1;}
  else if (waterLevel >=20 && waterLevel <=30){rendah=(30-waterLevel)/20;}
  else if (waterLevel >=30){rendah =0;}
  return rendah;
}
unsigned char Sedang () {
  if (waterLevel <=20) {sedang =0;}
  else if (waterLevel >=20&& waterLevel <=40){sedang=(waterLevel-20)/20;}
  else if (waterLevel >=50 && waterLevel <=70){sedang=(70-waterLevel)/20;}
  else if (waterLevel >=70){sedang =0;}
  return sedang;
}
unsigned char Tinggi () {
  if (waterLevel <=60) {tinggi =0;}
  else if (waterLevel >=60 && waterLevel <=80) {tinggi=(waterLevel-60)/20;}
  else if (waterLevel >=80) {tinggi=1;}
  return tinggi;
}
void fuzzifikasi() {
  //kelembapan
  Kering();
  Normal();
  Basah();
  //ketinggian Air
  Rendah();
  Sedang();
  Tinggi();
}

int ultrasonicDistance() {
  long duration, distance, jarak;
  digitalWrite(ultrasonicTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonicTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicEcho, LOW);
  duration = pulseIn(ultrasonicEcho , HIGH);
  distance = duration / 58.2;
  jarak = 100-distance;
  Serial.print("Ketinggian Air : ");
  Serial.print(jarak);
  Serial.println(" cm");
  return distance;
}

int sensorkelembapan() {
  int moisturePercentage, sensorMoistureValue;
  sensorMoistureValue = analogRead(soilMoisturePin);
  moisturePercentage= (sensorMoistureValue/1023)*100;
  Serial.print("Moisture Percentage = ");
  Serial.print(moisturePercentage);
  Serial.print("%\n");
  return moisturePercentage;
}

///////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  myservo1.attach(servoPin1);
  myservo2.attach(servoPin2);
  pinMode(ultrasonicTrig, OUTPUT);
  pinMode(ultrasonicEcho, INPUT);
}

void loop() {
  int waterLevelValue = ultrasonicDistance();
  int soilMoistureValue = sensorkelembapan();
  soilMoistureValue = analogRead(soilMoisturePin);
  Serial.print("kelembapan :");
  Serial.println(soilMoistureValue);
  
  if (soilMoistureValue <= 60 && waterLevelValue <= 30) {
    pos1 = 90;
    pos2 = 0;
  } else if (soilMoistureValue >= 60 && waterLevelValue <= 30) {
    pos1 = 0;
    pos2 = 0;
  } 
  if (soilMoistureValue >=30 && soilMoistureValue <=90 && waterLevelValue <=30) {
    pos1 = 0;
    pos2 = 0;
  } else if (soilMoistureValue >=60 && waterLevelValue >=20 && waterLevelValue <=70) {
    pos1 = 0;
    pos2 = 0;
  }
  if (soilMoistureValue >= 30 && waterLevelValue >=20 && waterLevelValue <= 70) {
    pos1 = 0;
    pos2 = 90;
  } else if (soilMoistureValue >= 30 && waterLevelValue >= 80) {
    pos1 = 0;
    pos2 = 90;
  } 
  
  myservo1.write(pos1);
  myservo2.write(pos2);
  delay(2000);
}



