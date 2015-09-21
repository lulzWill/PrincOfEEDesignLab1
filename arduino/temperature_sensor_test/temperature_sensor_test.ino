#include <Bridge.h>
#include <OneWire.h> 
#include <Console.h>
#include <FileIO.h>
#include <HttpClient.h>
#include <Mailbox.h>
#include <Process.h>
#include <YunClient.h>
#include <YunServer.h>
#include <SPI.h>

//Setup button
int buttonInt = 0;
int buttonPin = 3;

//Setup LEDs
int LEDone = 4;
int LEDtwo = 5;
int LEDthree = 6;
int LEDfour = 7;
int LEDfive = 8;
int LEDsix = 9;
int LEDseven = 10;
int numOfLEDs = 7;

//Temperature Sensor Pin
int tempPin = 2; //DS18S20 Signal pin on digital 2
int intTemp = 0;
String parametri = "";          //String of POST parameters

//IP Address of the sever on which there is the WS: http://www.mywebsite.com/
IPAddress server(52,3,191,89);

YunClient client;

//Temperature chip i/o
OneWire ds(tempPin); // on digital pin 2

void setup(void) {
 Bridge.begin();
 Serial.begin(9600);
 
 //Set pin modes
 pinMode(LEDone, OUTPUT);
 pinMode(LEDtwo, OUTPUT);
 pinMode(LEDthree, OUTPUT);
 pinMode(LEDfour, OUTPUT);
 pinMode(LEDfive, OUTPUT);
 pinMode(LEDsix, OUTPUT);
 pinMode(LEDseven, OUTPUT);

 //Attach interrupt
 attachInterrupt(buttonInt, displayLEDs, CHANGE);
}

//Interrupt function
void displayLEDs()
{
  if(digitalRead(buttonPin) == LOW)
  {
    displayOn();
  }
  else
  {
    allLightsOff();
  }
}

void displayOn() {
 for(int i=0; i<numOfLEDs; i++) 
   {
     int num = bitRead(intTemp, i);
     if(num == 1){
       digitalWrite(i+LEDone, HIGH);
     }
   }
}

void loop(void) {
 float temperature = getTemp();
 while(temperature == -1000)
 {
  allLightsOn();
  delay(1000);
  allLightsOff();
  delay(1000);
  temperature = getTemp();
 }

 if(isOn()) {
  displayOn();
 } else if(digitalRead(buttonPin) == HIGH){
  allLightsOff();
 }
 
 setTemp(temperature);
 Serial.println(temperature);

 temperature += 0.5;
 intTemp = (int) temperature;
 Serial.println(intTemp);
 Serial.println();
 delay(1000);
}

void allLightsOff(){
  for(int i = LEDone; i < LEDone + numOfLEDs; i++){
    digitalWrite(i, LOW);
  }
 return;
}

void allLightsOn(){
  for(int i = LEDone; i < LEDone + numOfLEDs; i++){
    digitalWrite(i, HIGH);
  }
 return;
}

float getTemp(){
 //returns the temperature from one DS18S20 in DEG Celsius

 byte data[12];
 byte addr[8];

 if ( !ds.search(addr)) {
   //no more sensors on chain, reset search
   ds.reset_search();
   return -1000;
 }

 if ( OneWire::crc8( addr, 7) != addr[7]) {
   Serial.println("CRC is not valid!");
   return -1000;
 }

 if ( addr[0] != 0x10 && addr[0] != 0x28) {
   Serial.print("Device is not recognized");
   return -1000;
 }

 ds.reset();
 ds.select(addr);
 ds.write(0x44,1); // start conversion, with parasite power on at the end

 byte present = ds.reset();
 ds.select(addr);  
 ds.write(0xBE); // Read Scratchpad

 
 for (int i = 0; i < 9; i++) { // we need 9 bytes
  data[i] = ds.read();
 }
 
 ds.reset_search();
 
 byte MSB = data[1];
 byte LSB = data[0];

 float tempRead = ((MSB << 8) | LSB); //using two's compliment
 float TemperatureSum = tempRead / 16;
 
 return TemperatureSum;
}

void setTemp(float temperature) {
  if (client.connect(server, 80)) {
    Serial.println("connected");

    parametri = "temp=" + String(temperature);
    client.println("POST /words.php HTTP/1.1");
    client.println("Host: princofeedesignlab1-tempsensor.rhcloud.com");
    client.print("Content-length:");
    client.println(parametri.length());
    Serial.println(parametri);
    client.println("Connection: Close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.println();
    client.println(parametri);
  } else {
    Serial.println("connection failed");
    delay(1000);
  }
  if (client.connected()) {
    client.stop();   //disconnect from server
  }
  delay(500);
}

bool isOn() {
  HttpClient client;
  client.get("http://princofeedesignlab1-tempsensor.rhcloud.com/ispressed.php");
  
  char c = client.read();
  Serial.println(c);
  if(c == 't') {
    return true;
  } else if(c == 'f'){
    return false;
  }
  else {
    Serial.println("isOn function error");
    return false;
  }
}

