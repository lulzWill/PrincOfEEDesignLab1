#include <OneWire.h> 

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

//Temperature chip i/o
OneWire ds(tempPin); // on digital pin 2

void setup(void) {
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
    for(int i=0; i<numOfLEDs; i++) 
    {
      int num = bitRead(intTemp, i);
      if(num == 1){
        digitalWrite(i+4, HIGH);
      }
    }
  }
  else
  {
    allLightsOff();
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
