/* 
  Sketch generated by the Arduino IoT Cloud Thing "Untitled 2"
  https://create.arduino.cc/cloud/things/a67d27ae-171c-49ea-9cf0-452ca4a3d991 

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  CloudLength distance;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#define ONBOARD_LED 2

#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


//////////////////////////////////////// Dabble
//#define CUSTOM_SETTINGS;
//#define INCLUDE_SENSOR_MODULE
//#define INCLUDE_DATALOGGER_MODULE
//bool isFileOpen = true;
//#include <DabbleESP32.h>

//////////////////////////////////////// WiFi
#ifdef ESP32
  #include <WiFi.h>
  //#include <HTTPClient.h>
#else
  //#include <ESP8266WiFi.h>
  //#include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#endif
// Replace with your network credentials
const char* ssid     = "Fibertel WiFi514 2.4GHz";
const char* password = "0043106872";
//const char* serverName = "http://192.168.0.217:3000/water";
//const char* serverName = "http://en5yo8is2947r1k.m.pipedream.net";
//const char* serverName = "http://requestbin.net/r/4d5xglfd";

#include <PubSubClient.h>
const char* mqtt_server = "147.182.212.87";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//////////////////////////////////////// Dabble
//void initializeFile() {
  //Serial.println("Initialize");
//  DataLogger.createFile("Water level");
//  DataLogger.createColumn("Distance");
//}

//////////////////////////////////////// Ultrasonic sensor
const int trigPin = 9;
const int echoPin = 10;
#define SOUND_VELOCITY 0.034 // define sound velocity in cm/uS
#define CM_TO_INCH 0.393701
long duration;
float distanceCm;
//define PI 3.1415926535897932384626433832795
float pi = 3.14159265359;
long distance;

//float waterHeight;
//float tankHeight = 52; //enter your water tank height here in [cm]
//float totalVolume = 2613; // volume here is in [cm^3], change depending on tank volume
//float waterQuantity;
//float volume;

void setup() {
  // Initialize serial and wait for port to open:
  //Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  //////////////////////////////////////// Ultrasonic sensor
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  dht.begin();

  //////////////////////////////////////// Dabble
  //Dabble.begin("MyESP32");    //set bluetooth name of your device
  //DataLogger.sendSettings(&initializeFile);

  WiFi.begin(ssid, password);
  //Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    //Serial.print(".");
  }
  /*Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());*/
  // LED
  pinMode(ONBOARD_LED,OUTPUT);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}

void loop() {
  if (!client.connected()) { 
    reconnect();
  }
  client.loop();

  //////////////////////////////////////// Dabble
  //Dabble.processInput();
  // Your code here 

  //////////////////////////////////////// Ultrasonic sensor
  digitalWrite(trigPin, LOW); // Clears the trigPin
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH); // Sets the trigPin on HIGH state for 10 micro seconds
  delayMicroseconds(20);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  distanceCm = duration * SOUND_VELOCITY/2; // Calculate the distance
  //distance = distanceCm;

  //Serial.println(distanceCm);
  //return;

  //////////////////////////////////////// Dabble
  //if (isFileOpen == true) {
    //print_Sound_data();
    //DataLogger.send("Distance", String(distanceCm));
  //}
  /*if((digitalRead(closeFileSignalPin) == LOW) && isFileOpen == true)
  {
    isFileOpen = false;
    DataLogger.stop();
  }*/

  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();


  /*Serial.println("Humidity test");
  Serial.println(h);
  Serial.println("Temperature test");
  Serial.println(t);*/

  if (isnan(h) || isnan(t)) {
    //Serial.println(F("Failed to read from DHT sensor!"));
    return;
    delay(1000);
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  /*Serial.print(F("Distance: "));
  Serial.println(distanceCm);
  Serial.print(F("  Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(F("°C  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));*/

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    char distanceString[8];
    dtostrf(distanceCm, 1, 2, distanceString);

    char temperatureString[8];
    char humidityString[8];
    char heatIndexString[8];

    dtostrf(t, 1, 2, temperatureString);
    dtostrf(h, 1, 2, humidityString);
    dtostrf(hic, 1, 2, heatIndexString);

//    Serial.println(duration);
    

    // El volumen de un cilindro es π r² h, y el área de su superficie es 2π r h + 2π r².
    //float water_volume = PI 
    
    //Serial.print("Water level: ");
    //Serial.println(distanceString);
    client.publish("home/patio/water_height", distanceString);

    client.publish("home/patio/temperature", temperatureString);
    client.publish("home/patio/humidity", humidityString);
    client.publish("home/patio/heat_index", heatIndexString);
  }

  if (WiFi.status() == WL_CONNECTED) {
    //////////////////////////////////////// LED
    digitalWrite(ONBOARD_LED,HIGH);
  
    WiFiClient client;
  }

  delay(1000);
}

void callback(char* topic, byte* message, unsigned int length) {
  //Serial.print("Message arrived on topic: ");
  //Serial.print(topic);
  //Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    //Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  //Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  /*if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }*/
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      //Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/visualize-esp32-esp8266-sensor-readings-from-anywhere/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

*/


/*
const int trigPin = 9;
const int echoPin = 10;

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#endif

//#include <Wire.h>

// Replace with your network credentials
const char* ssid     = "Fibertel WiFi514 2.4GHz";
const char* password = "0043106872";

// REPLACE with your Domain name and URL path or IP address with path
//const char* serverName = "http://en5yo8is2947r1k.m.pipedream.net";
const char* serverName = "http://192.168.0.217:3000/water";
//const char* serverName = "http://en5yo8is2947r1k.m.pipedream.net";

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-data.php also needs to have the same key 
String apiKeyValue = "tPmAT5Ab3j7F9";

//Adafruit_BME280 bme;  // I2C
//Adafruit_BME280 bme(BME_CS);  // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);  // software SPI

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    

  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;
  
  // Prints the distance on the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  //Serial.print("Distance (inch): ");
  //Serial.println(distanceInch);
  
  //delay(1000);


    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    
    // Specify content-type header
    //http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepare your HTTP POST request data
    //String httpRequestData = "distance=" + String(distanceCm) + "";
    //String httpRequestData = "api_key=" + apiKeyValue + "&value1=" + String(distanceCm) + "";
    //String httpRequestData = "api_key=tPmAT5Ab3j7F9&value1=24.75&value2=49.54&value3=1005.14";
    
    
    // You can comment the httpRequestData variable above
    // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
    //String httpRequestData = "api_key=tPmAT5Ab3j7F9&value1=24.75&value2=49.54&value3=1005.14";

    // Send HTTP POST request
    //int httpResponseCode = http.POST(httpRequestData);
     
    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");
    
    // If you need an HTTP request with a content type: application/json, use the following:
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"distance\":\"" + String(distanceCm) + "\",\"duration\":\""+ String(duration) +"\",\"value3\":\"78\"}");

    //Serial.print("httpRequestData: ");
    
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  //Send an HTTP POST request every 30 seconds
  delay(5000);  
}
*/

/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-hc-sr04-ultrasonic-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/
/*
const int trigPin = 9;
const int echoPin = 10;

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

void setup() {
  Serial.begin(115200); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;
  
  // Prints the distance on the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);
  
  delay(1000);
}
*/







/*
#include <HCSR04.h>

HCSR04 hc(9, 10); //initialisation class HCSR04 (trig pin , echo pin)

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    Serial.println(hc.dist()); // return curent distance in serial
    delay(60);                 // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
}
*/
