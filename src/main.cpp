//P0.15 is a built in led, on the nice nano clones from aliexpress it is red.

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED PIN_015 //Set a definiton on pin P0.15 called "LED".

Adafruit_SSD1306 display(128, 32, &Wire, -1);

void setup() {
    pinMode(LED, OUTPUT); //Set the LED to output mode.

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { //if error then builtin led light up
        digitalWrite(LED, HIGH); //Set the LED to high
        while (true);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0); //place where the text will be drawn
    display.println("By ICantMakeThings");
    display.setCursor(0, 16);
    display.println("And cat5e-inv");
    display.display();
}

void loop() {
  digitalWrite(LED, LOW); //Set the LED to low
  delay(100); //wait 100ms

  digitalWrite(LED, HIGH); //Set the LED to high
  delay(100); //wait 100ms
}