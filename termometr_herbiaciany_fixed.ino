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

#define BUFFER_SIZE 5

// Data wire is plugged into pin 7 on the Arduino
#define ONE_WIRE_BUS 7
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DHT dht(DHTPIN, DHTTYPE);

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

float temp_array[BUFFER_SIZE];
float diff_array[BUFFER_SIZE - 1];
float temperature = 0.0;
float prev_temperature;
float setpoint0;
float setpoint_buffer;

float temperature_dht11 = 0;
float humidity_dht11 = 0;
bool DS18B20_OK, DHT11_OK;
bool setpoint_enable = 0, setpoint_enable_last;

bool parity = 0;
unsigned long melody_start;
bool melody_playing;

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

  // Display welcome text - not working, I don't know why
  /*
    delay(2000);
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Termometr Herbaciany v2.0");
    display.println("GPIO OK");
    display.println("Ekran OK");
    display.println("Termometr OK");
    display.display();
    display.clearDisplay();
    delay(2000);
  */

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

  setpoint0  = map(analogRead(0), 0, 1023, 10, 70);
  sensors_read_display(true);
}

void loop() {
  if (!melody_playing)
    sensors_read_display(!setpoint_enable);

  if (!melody_playing)
  {
    if ( (millis() > 1000 ) && (millis() % 400 < 3))
    {
      printDisplay();
      delay(3);
    }
  }
  
  if ( temp_array[1] >= setpoint0 && temp_array[0] < setpoint0 && millis() > 4000 && !melody_playing)
  {
    melody_playing = 1;
    melody_start = millis();
  }
  melody3(buzzer_pin, melody_start);
  if (!melody_playing)
    read_button_neg_switch(button_pin, setpoint_enable);


  if (setpoint_enable == 1)
    setpoint_buffer = map(analogRead(0), 0, 1023, 10, 70);

  if (setpoint_enable == 0 && setpoint_enable_last)
    setpoint0 = setpoint_buffer;

  // remember last state for pulse event
  setpoint_enable_last = setpoint_enable;
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

void sensors_read_display(bool read)
{
  if (millis() % 1000 < 2)
  {
    if (read)
    {
      sensors.requestTemperatures();
      temperature = sensors.getTempCByIndex(0);


      humidity_dht11 = dht.readHumidity();
      temperature_dht11 = dht.readTemperature();
      cicrBuffer(temp_array, temperature, BUFFER_SIZE);
      differentalArray(diff_array, temp_array, BUFFER_SIZE - 1);

      for ( int i = 0; i < BUFFER_SIZE; i++)
      {
        Serial.print(temp_array[i]);
        Serial.print('\t');
      }
      Serial.println();
      for ( int i = 0; i < BUFFER_SIZE - 1; i++)
      {
        Serial.print(diff_array[i]);
        Serial.print('\t');
      }
      Serial.println("\n-----------------");
    }
    else
      Serial.println("\nRead blocked, setpoint mode");
  }
  delay(2);
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

  if (isnan(humidity_dht11) && isnan(temperature_dht11) )
    display.setTextSize(2);
  else
    display.setTextSize(1);
  display.setTextColor(WHITE);
  //display.setCursor(0, 24);
  display.print("SP ");
  char str[6];
  if (setpoint_enable == 0)
  {
    dtostrf(setpoint0, 2, 1, str );
    display.print(str);
    display.print(" \tC");
  }

  if ( setpoint_enable == 1 && parity == 1 )
  {
    dtostrf(setpoint_buffer, 2, 1, str );
    display.print(str);
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
  if (isnan(humidity_dht11) && isnan(temperature_dht11) )
    display.setTextSize(2);
  else
    display.setTextSize(1);
  display.print("T0 ");
  char str[6];
  dtostrf(temperature, 2, 1, str );
  display.print(str);
  display.println(" \tC");

  if (!isnan(humidity_dht11) || !isnan(temperature_dht11) )
  {
    display.print("T1 : ");
    display.print(temperature_dht11);
    display.println(" \tC");

    display.print("H1 : ");
    display.print(humidity_dht11);
    display.println(" %");
  }

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

  melody_playing = 0;
}

void cicrBuffer(float cBuffer[], float push_input, short buf_length)
{
  for (short i = buf_length - 1 ; i >= 0; i--)
  {
    cBuffer[i] = cBuffer[i - 1];
  }
  cBuffer[0] = push_input;
}

void differentalArray(float diff_array[], float input_array[], short array_length)
{
  for (short i = array_length - 1; i >= 0; i--)
  {
    diff_array[i] = input_array[i] - input_array[i + 1];
  }
}


