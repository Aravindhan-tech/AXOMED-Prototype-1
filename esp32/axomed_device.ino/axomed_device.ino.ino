#include <WiFi.h>
#include <HTTPClient.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PulseSensorPlayground.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// WiFi
const char* ssid = "OPPO";
const char* password = "amma2appa";

String serverName = "http://10.185.252.1:3000/sensor";

// DHT11
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// DS18B20
#define ONE_WIRE_BUS 15
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature bodySensor(&oneWire);

// Pulse Sensor
const int PulseWire = 34;
const int Threshold = 550;
PulseSensorPlayground pulseSensor;

void setup() {

Serial.begin(115200);

// Start sensors
dht.begin();
bodySensor.begin();

// Pulse sensor setup
pulseSensor.analogInput(PulseWire);
pulseSensor.setThreshold(Threshold);
pulseSensor.begin();

// OLED start
if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
Serial.println("OLED not found");
for(;;);
}

display.clearDisplay();

// WiFi connect
WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(1000);
Serial.println("Connecting to WiFi...");
}

Serial.println("WiFi Connected");

}

void loop() {

// Read sensors
float humidity = dht.readHumidity();
float roomTemp = dht.readTemperature();

bodySensor.requestTemperatures();
float bodyTemp = bodySensor.getTempCByIndex(0);

int myBPM = pulseSensor.getBeatsPerMinute();

// OLED Display
display.clearDisplay();

display.setTextSize(2);
display.setTextColor(WHITE);
display.setCursor(10,0);
display.println("AXOMED");

display.setTextSize(1);

display.setCursor(0,20);
display.print("Room:");
display.print(roomTemp);
display.println("C");

display.setCursor(0,30);
display.print("Hum :");
display.print(humidity);
display.println("%");

display.setCursor(0,40);
display.print("Body:");
display.print(bodyTemp);
display.println("C");

display.setCursor(0,50);
display.print("BPM :");
display.print(myBPM);

display.display();

// Send data to backend
if(WiFi.status() == WL_CONNECTED){

HTTPClient http;

http.begin(serverName);
http.addHeader("Content-Type", "application/json");

String jsonData = "{";
jsonData += "\"roomTemp\":" + String(roomTemp) + ",";
jsonData += "\"humidity\":" + String(humidity) + ",";
jsonData += "\"bodyTemp\":" + String(bodyTemp) + ",";
jsonData += "\"bpm\":" + String(myBPM);
jsonData += "}";

int httpResponseCode = http.POST(jsonData);

Serial.print("HTTP Response: ");
Serial.println(httpResponseCode);

http.end();

}

delay(2000);

}

