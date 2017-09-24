#include <Wire.h>
#include "TSL2561.h"
#include <DHT.h>
#include <Wire.h>
#include <RTClib.h>
#define CH1 8   // Connect Digital Pin 8 on Arduino to CH1 on Relay Module
#define CH3 7   // Connect Digital Pin 7 on Arduino to CH3 on Relay Module
#define CH2 6
#define LEDgreen 4 //Connect Digital Pin 4 on Arduino to Green LED (+ 330 ohm resistor) and then to "NO" terminal on relay module
#define LEDyellow 12 //Connect Digital Pin 12 on Arduino to Yellow LED (+ 330 ohm resistor) and then to "NC" terminal on relay module
 


//Constants
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor for normal 16mhz Arduino

//Variables
float hum;  //Stores humidity value
float temp; //Stores temperature value
int redPin = 12;    // Red LED connected to digital pin 12 to indicate it s time for watering
int greenPin = 11;  // Green LED connected to digital pin 11 to indicate moistur is in good level
int lag = 0;
int motorPin = A1; // this the pin that turns the water pump on.

RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};



TSL2561 tsl(TSL2561_ADDR_FLOAT); 

void setup(void) {
  Serial.begin(9600);
  

tsl.begin();
tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)
tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
 if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    dht.begin();
    pinMode(redPin, OUTPUT);        // sets the digital pin as output
    pinMode(greenPin, OUTPUT);      // sets the digital pin as output
    pinMode(motorPin, OUTPUT);      // sets the analogue pin as output
    pinMode(CH1, OUTPUT);
   pinMode(CH3, OUTPUT);
   pinMode(CH2, OUTPUT);

    digitalWrite(CH1,LOW);
   digitalWrite(CH3,LOW);
   digitalWrite(CH2,LOW);
    
}

void loop(void) {
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  Serial.print("IR: "); Serial.print(ir);   Serial.print("\t\t");
  Serial.print("Full: "); Serial.print(full);   Serial.print("\t");
  Serial.print("Visible: "); Serial.print(full - ir);   Serial.print("\t");
  
  Serial.print("Lux: "); Serial.println(tsl.calculateLux(full, ir));
  
  
   DateTime now = rtc.now();
    Serial.print("Measurement time is: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp= dht.readTemperature();
    
    //Print temp and humidity values to serial monitor
    int val;
    val = analogRead(0); //connect sensor to Analog 0
    Serial.print("Soil moisture: ");
    Serial.print(val); //print the value to serial port
    Serial.print(", Room Humidity: %");
    Serial.print(hum);
    Serial.print(", Room Temperature: ");
    Serial.print(temp);
    Serial.println(" Celsius");
   

  if (val<400)  // Ths is an imaginary level for soil moisture
  {
    Serial.println("I need water!"); // This line send a mesage into serial monitor
    Serial.println();
    digitalWrite(redPin, HIGH); // Ths line turns on the red LED to hazard
    digitalWrite(greenPin, LOW); // This line keeps green LED off.
    digitalWrite(CH1, HIGH);
  } 
  else
    {
      Serial.println("I am OK, Thanks :)"); // This line send a mesage into serial monitor
      Serial.println();
      digitalWrite(redPin, LOW); // Ths line keeps red LED off.
      digitalWrite(greenPin, HIGH); // This line turns green LED on.
      digitalWrite(CH1, LOW);
      }
    if (temp < 19)
    {
      digitalWrite(CH2, LOW);  //turns A/C system on
      lag=8000;
    }
    else
    {
      digitalWrite(CH2, HIGH);
      lag=4000;
    }
    if (tsl.calculateLux(full, ir) < 40)
    {
      digitalWrite(CH3, HIGH);  // turns lights on
    }
    else
    {
      digitalWrite(CH3, LOW);
     }
    
    delay(lag);
}

