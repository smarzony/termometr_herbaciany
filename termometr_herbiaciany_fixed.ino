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


#define BUTTON_DELAY 400
#define G3 196
#define E3 165
#define C3 131
#define duration 150
#define pause 200

#define buzzer_pin 5
#define led 13
#define button_pin 9

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
float setpoint0 = 55.00;
float temperature_dht11 = 0;
float humidity_dht11 = 0;
bool DS18B20_OK, DHT11_OK;
bool setpoint_enable = 0;
bool parity = 0;
unsigned long melody_start;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting system");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  Serial.println("OLED started");

  pinMode(led, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(button_pin, INPUT_PULLUP);

  digitalWrite(buzzer_pin, HIGH);
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

  if (millis() % 400 < 2)
    printDisplay();

  if ( prev_temperature >= setpoint0 && temperature < setpoint0 && millis() > 4000)
  {
    melody_start = millis();
  }
  melody3(buzzer_pin, melody_start);
  read_button_neg_switch(button_pin, setpoint_enable);
  if (setpoint_enable == 1)
    setpoint0 = map(analogRead(0), 0, 1023, 10, 70);

  prev_temperature = temperature;
}

void read_button_neg_switch(byte button, bool &state)
{
  static unsigned long lastTime;
  unsigned long timeNow = millis();

  if (digitalRead(button) == 0) {
    if (timeNow - lastTime < BUTTON_DELAY)
      return;
    if (digitalRead(button) == 0)
    {
      state = !state;
    }
    lastTime = timeNow;
  }
}

void sensors_read_display()
{
  if (millis() % 1000 == 0)
  {
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);

    humidity_dht11 = dht.readHumidity();
    temperature_dht11 = dht.readTemperature();
  }
}

void printDisplay()
{
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

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 24);
  display.print("SP : ");

  if (setpoint_enable == 0 || setpoint_enable == 1 && parity == 1)
  {
    display.print(setpoint0);
    display.print(" \tC");
  }

  tempDisplay();
  display.display();
  display.clearDisplay();
  parity = !parity;
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
  digitalWrite(buzzer_pin, HIGH);
}


