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
float previousTemp = 0;
float temperature = 0;
String parametri = "";          //String of POST parameters

//IP Address of the sever on which there is the WS: http://www.mywebsite.com/
IPAddress server(52, 3, 191, 89);

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

  HttpClient client;
  char c = 'a';
  while (c != 't' && c != 'f')
  {
    client.get("http://princofeedesignlab1-tempsensor.rhcloud.com/ispressed.php");
    c = client.read();
  }

  allLightsOn();
  delay(100);
  allLightsOff();
  delay(100);
  allLightsOn();
  delay(100);
  allLightsOff();
  delay(100);
  allLightsOn();
  delay(100);
  allLightsOff();
}

//Interrupt function
void displayLEDs()
{
  if (digitalRead(buttonPin) == LOW)
  {
    displayOn();
  }
  else
  {
    allLightsOff();
  }
}

void displayOn() {
  for (int i = 0; i < numOfLEDs; i++)
  {
    int num = bitRead(intTemp, i);
    if (num == 1) {
      digitalWrite(i + LEDone, HIGH);
    }
  }
}

void loop(void) {
  temperature = getTemp();
  if (temperature == -1000)
  {
    HttpClient client;
    parametri = "http://princofeedesignlab1-tempsensor.rhcloud.com/ispressed.php?action=unplugged";
    client.get(parametri);

    while (temperature == -1000) {
      parametri = "http://princofeedesignlab1-tempsensor.rhcloud.com/words.php?temp=null";
      client.get(parametri);

      allLightsOn();
      delay(1000);
      allLightsOff();
      delay(1000);
      temperature = getTemp();
    }

    parametri = "http://princofeedesignlab1-tempsensor.rhcloud.com/ispressed.php?action=plugged";
    client.get(parametri);

  }

  if (isOn()) {
    displayOn();
  } else if (digitalRead(buttonPin) == HIGH) {
    allLightsOff();
  }

  //  if (((previousTemp - temperature) > 20) || ((previousTemp - temperature) > -20))
  if ((temperature - previousTemp < 20.0) && (temperature - previousTemp > -20.0))
  {
    setTemp(temperature);
    Serial.println("Sent: " + String(temperature));

    temperature += 0.5;
    intTemp = (int) temperature;
    temperature -= 0.5;
  }
  Serial.println("Previous: " + String(previousTemp));
  Serial.println("Current: " + String(temperature));
  Serial.println();
  previousTemp = temperature;
}

void allLightsOff() {
  for (int i = LEDone; i < LEDone + numOfLEDs; i++) {
    digitalWrite(i, LOW);
  }
  return;
}

void allLightsOn() {
  for (int i = LEDone; i < LEDone + numOfLEDs; i++) {
    digitalWrite(i, HIGH);
  }
  return;
}

float getTemp() {
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
  ds.write(0x44, 1); // start conversion, with parasite power on at the end

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
  HttpClient client;
  parametri = "http://princofeedesignlab1-tempsensor.rhcloud.com/words.php?temp=" + String(temperature);
  client.get(parametri);
}
bool isOn() {
  HttpClient client;
  client.get("http://princofeedesignlab1-tempsensor.rhcloud.com/ispressed.php");

  char c = client.read();
  if (c == 't') {
    return true;
  } else if (c == 'f') {
    return false;
  }
  else {
    Serial.println("isOn function error");
    return false;
  }
}