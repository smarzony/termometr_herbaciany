/*
  Name:    termometr_herbaciany.ino
  Created: 17.06.2018 20:52:32
  Author:  Piotr
*/

#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11

// Data wire is plugged into pin 7 on the Arduino
#define ONE_WIRE_BUS 7
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DHT dht(DHTPIN, DHTTYPE);

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

float temperature = 0;
float temperature_dht11 = 0;
float humidity_dht11 = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting system");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  Serial.println("OLED started");

  pinMode(13, OUTPUT);
  Serial.println("GPIO started");

  dht.begin();
  Serial.println("DHT11 started");

  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  delay(200);

  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  delay(200);

  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  delay(200);
}

void loop() {

  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  humidity_dht11 = dht.readHumidity();
  temperature_dht11 = dht.readTemperature();

  if (isnan(humidity_dht11) || isnan(temperature_dht11) )
  {
    Serial.println("Failed to read from DHT sensor!");
  }

  // Compute heat index in Fahrenheit (the default)
  float hic = dht.computeHeatIndex(temperature_dht11, humidity_dht11, false);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 24);

  if (temperature < 70.0 && temperature > 60.0)
    display.println("< 70 \tC");

  if (temperature < 60.0 && temperature > 51.0)
    display.println("< 60 \tC");

  if (temperature < 50.0)
    display.println("Insert Honey");

  staticElements();
  tempDisplay();

  display.display();
  delay(200);
  display.clearDisplay();
}

void tempDisplay()
{
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("T0 : ");
  display.print(temperature);
  display.println(" \tC");

  display.print("T1 : ");
  display.print(temperature_dht11);
  display.println(" \tC");

  display.print("H1 : ");
  display.print(humidity_dht11);
  display.println(" %");

}

void staticElements()
{

}

