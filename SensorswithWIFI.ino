 /*CET/CSC 492 Senior Project
   Kory Bilak - Computer Science
   Kiana Savage - Computer Engineering Technology
 */
 
 /*  PINOUTS
        Temperature and Humidity Sensor
         PIN 4 - CL
         PIN 5 - DA
         Light Sensor
         PIN 14 (SCK)
         Soil Mositure
         PIN 13 (MOSI)
 */
 
 //Library for WiFi CConnection
 #include <ESP8266WiFi.h>
 #include <Ethernet.h>
 #include <utility/util.h>
 
 
 //Libraries for temperature and humidity sensor
 #include "SparkFun_Si7021_Breakout_Library.h"
 #include <Wire.h>
 
 //Initialization for WiFi Connection variables
 const char* ssid     = "CSC492";
 const char* password = "";
 const char* host = "192.168.0.102";
 int value = 0;
 
 //Initialization for Temp/humidity sensor variables
 int tempfRHReading = 0;             //Value for storing     36 temp/humidity value
 int tempfRHPin = 4;                 //output pin on feather huzzah
 float humidity = 0;
 float tempf = 0;
 int power = 3;
 int GND = 2;
 
 //Initialization for photocell sensor variables
 int photocellPin = 14;              // the cell and 100K pulldown are connected to a14 GPTO SCK
 int photocellReading = 0;           // the analog reading from the analog resistor divider
 
 //Rather than powering the sensor through the 3.3V or 5V pins,
 //we'll use a digital pin to power the sensor. This will
 //prevent corrosion of the sensor as it sits in the soil.
 //Initialization for soil moisture sensor variables
 int soilReading = 0;                //value for storing moisture value
 int soilPin = 13;                   //Declare a variable for the soil moisture sensor PIN 13 MOSI
 int soilPower = 5;                  //Variable for Soil moisture Power
 
 struct sensor_readings {
   int32_t temperature;
   int32_t humidity;
   int32_t light;
   int32_t soil_moisture;
 } sensor_readings;
 
 //Create Instance of SI7021 temp and humidity sensor
 Weather sensor;

   void setup()
 {
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

   Serial.begin(9600);             // open serial over USB

   // initialize digital pin LED_BUILTIN as an output.
   pinMode(LED_BUILTIN, OUTPUT);

  //Temperature and Humidity
   pinMode(power, OUTPUT);
   pinMode(GND, OUTPUT);
 
   digitalWrite(power, HIGH);
   tempfRHReading = analogRead(tempfRHPin);
   digitalWrite(GND, LOW);
 
   //Initialize the I2C sensors and ping them
   sensor.begin();
 
 
   //Soil Moisture
   pinMode(soilPower, OUTPUT);      //Set D7 as an OUTPUT
   digitalWrite(soilPower, LOW);    //Set to LOW so no power is flowing through the sensor
 
 }

 void loop()
 {
   delay(5000);
   ++value;
 
   Serial.print("connecting to ");
   Serial.println(host);
 
   // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 9876;
   if (!client.connect(host, httpPort)) {
     Serial.println("connection failed");
     //return;
  }

   //delay for heartbeat
   digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
   delay(1000);                       // wait for a second
   digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
   delay(1000);                       // wait for a second
 
   mainTempHum();
   //Call function to get content from temp and humidity sensor
   //delay(1000);
   mainPhotocell();
   //Call function to get content from light sensor
   //delay(1000);
   mainSoil();
   //Call function to get content from soil moisture sensor
   //delay(1000);
 
 
 
   // Read all the lines of the reply from server and print them to Serial //
   struct sensor_readings reading;
 
  tempf = tempf * 100;
   reading.temperature = htonl((int32_t)tempf);
   humidity = humidity * 100;
   reading.humidity = htonl((int32_t)humidity);
   reading.light = htonl(photocellReading);
   reading.soil_moisture = htonl(soilReading);
 
  client.write((byte*)&reading, sizeof(reading));


   Serial.println();
   Serial.println("closing connection");
   delay(5000);
}

 //This is a function used to get the temp and humidity content
 void mainTempHum()
 {
 //Get readings from all sensors
   getWeather();
   printInfo();
   delay(1000);
 }

 //This is a function used for the temperature and humidity readings
 void getWeather()
 {
   // Measure Relative Humidity from the Si7021 sensor
   humidity = sensor.getRH();
   // Measure Temperature from the Si7021 sensor
   tempf = sensor.getTempF();
   // Temperature is measured every time RH is requested.
   // It is faster, therefore, to read it from previous RH
   // measurement with getTemp() instead with readTemp()
 }
 
 //This function prints the weather data out to the default Serial Port
 void printInfo()
 { Serial.print("Temp:");
   Serial.print(tempf);
   Serial.print("F, ");
 
   Serial.print("Humidity:");
   Serial.print(humidity);
   Serial.println("%");
 }
 
 //This is a function used to get the photocell content
void mainPhotocell()
 {
   photocellReading = analogRead(photocellPin);
   Serial.print("Light Sensor = ");
   Serial.print(photocellReading);     // the raw analog reading
 
   // We'll have a few threshholds, qualitatively determined
   if (photocellReading < 10)
   { Serial.println(" - Dark");
   } else if (photocellReading < 1000) {
     Serial.println(" - Light");
   } else {
     Serial.println(" - Very bright");
   }
   delay(1000);
 }
 
 //This is a function used to get the soil moisture content
 void mainSoil()
 { Serial.print("Soil Moisture = ");
   //get soil moisture value from the function below and print it
   Serial.println(readSoil());

  //This 1 second timefrme is used so you can test the sensor and see it change in real-time.
  //For in-plant applications, you will want to take readings much less frequently.
  delay(1000);//take a reading every second
 }
 
 //This is a function used for the power and sensor readings
 int readSoil()
 { digitalWrite(soilPower, HIGH);//turn D7 "On"
   delay(10);//wait 10 milliseconds
   soilReading = analogRead(soilPin);//Read the SIG value form sensor
   digitalWrite(soilPower, LOW);//turn D7 "Off"
   return soilReading;//send current moisture value
 }
 //END OF PROGRAM
