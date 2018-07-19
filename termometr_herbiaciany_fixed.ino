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

#define perfect_temp 55.00

#define G3 196
#define E3 165
#define C3 131
#define duration 150
#define pause 200

#define buzzer_pin 5
#define led 13
#define button 9

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
float prev_temperature;
float temperature_dht11 = 0;
float humidity_dht11 = 0;
bool DS18B20_OK, DHT11_OK;
unsigned long melody_start;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting system");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  Serial.println("OLED started");

  pinMode(led, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  Serial.println("GPIO started");

  dht.begin();
  Serial.println("DHT11 started");

  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  delay(200);

  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  delay(200);

  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  delay(200);
}

void loop() {
  sensors_read_display();
  if ( prev_temperature >= perfect_temp && temperature < perfect_temp && millis() > 4000)
  {
    melody_start = millis();
  }
  melody3(buzzer_pin, melody_start);
  prev_temperature = temperature;
}

void melody3(short buzzer, unsigned long start_melody)
{
  unsigned long now = millis();
  if (now - start_melody > (duration * 0 + pause * 0) && now - start_melody < (duration * 1 + pause * 0))
  {
    tone(buzzer, E3);
    digitalWrite(led, HIGH);
  }
  if (now - start_melody > (duration * 1 + pause * 0) && now - start_melody < (duration * 1 + pause * 1))
  {
    noTone(buzzer);
    digitalWrite(led, LOW);
  }

  if (now - start_melody > (duration * 1 + pause * 1) && now - start_melody < (duration * 2 + pause * 1))
  {
    tone(buzzer, G3);
    digitalWrite(led, HIGH);
  }
  if (now - start_melody > (duration * 2 + pause * 1) && now - start_melody < (duration * 2 + pause * 2))
  {
    noTone(buzzer);
    digitalWrite(led, LOW);
  }

  if (now - start_melody > (duration * 2 + pause * 2) && now - start_melody < (duration * 3 + pause * 2))
  {
    tone(buzzer, E3);
    digitalWrite(led, HIGH);
  }
  if (now - start_melody > (duration * 3 + pause * 2) && now - start_melody < (duration * 3 + pause * 3))
  {
    noTone(buzzer);
    digitalWrite(led, LOW);
  }
}

void sensors_read_display()
{
  if (millis() % 200 == 0)
  {
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);

    humidity_dht11 = dht.readHumidity();
    temperature_dht11 = dht.readTemperature();

    if (isnan(humidity_dht11) || isnan(temperature_dht11) )
    {
      Serial.println("DHT11 error!");
      DHT11_OK = 0;
    }
    else
      DHT11_OK = 1;

    if (temperature == -127.0)
    {
      Serial.println("DS18B20 error!");
      DS18B20_OK = 0;
    }
    else
      DS18B20_OK = 1;

    // Compute heat index in Fahrenheit (the default)
    float hic = dht.computeHeatIndex(temperature_dht11, humidity_dht11, false);

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 24);

    if (temperature < 70.0 && temperature > 60.0)
    {
      if (DS18B20_OK)
      {
        display.println("< 70 \tC");
        Serial.println("< 70 \tC");
      }

    }

    if (temperature < 60.0 && temperature > 51.0)
    {
      if (DS18B20_OK)
      {
        display.println("< 60 \tC");
        Serial.println("< 60 \tC");
      }
    }

    if (temperature < 50.0 && temperature != -127.0)
    {
      display.println("Insert Honey");
      Serial.println("Insert Honey");
    }

    staticElements();
    tempDisplay();

    digitalWrite(buzzer_pin, digitalRead(button));

    display.display();
    // delay(200);
    display.clearDisplay();
  }
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

