//P0.15 is a built in led, on the nice nano clones from aliexpress it is red.

#include <Arduino.h>
#include <Adafruit_TinyUSB.h> // for Serial
#include "bluefruit.h"
#include "nrf_soc.h"
#include "nrf_sdm.h"

// #include <bluemicro_nrf52.h>
// #include <bluemicro_hid.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// #include <Adafruit_SH1106.h>
#include <Adafruit_SH110X.h>

// #include <U8g2lib.h>

#include <AccelStepper.h>

#define LED PIN_015 //Set a definiton on pin P0.15 called "LED".

#define USE_BREAK

#define AXIS_2_FORWARD_PIN PIN_017
#define AXIS_2_BACKWARD_PIN PIN_020

#define STEPPER_DIR_PIN PIN_100 //Set a definiton on pin P0.09 called "STEPPER_DIR_PIN".
#define STEPPER_STEP_PIN PIN_024 //Set a definiton on pin P0.10 called "STEPPER_STEP_PIN".
#define STEPPER_ENABLE_PIN PIN_022 //Set a definiton on pin P0.11 called "STEPPER_ENABLE_PIN".
// #define STEPPER_RST_PIN PIN_113 //Set a definiton on pin P0.13 called "STEPPER_RESET_PIN".

#define STEPS 200
AccelStepper stepper(AccelStepper::DRIVER, STEPPER_STEP_PIN, STEPPER_DIR_PIN);
// Stepper stepper(STEPS, STEPPER_DIR_PIN, STEPPER_STEP_PIN);
// #define motorInterfaceType 1

// Adafruit_SSD1306 display(128, 64, &Wire, -1);
#define OLED_RESET -1
// Adafruit_SH1106 display(OLED_RESET);
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, OLED_RESET);
// BLEUart bleuart;  // BLE UART сервіс

const uint8_t LED_SERVICE_UUID[] =
{
    0x6C, 0xCE, 0x98, 0x91, 0xB9, 0xB3, 0x11, 0x87,
    0x9E, 0x47, 0xF5, 0x67, 0x01, 0x00, 0x4B, 0xE5
};

const uint8_t LED_CHR_UUID[] =
{
    0x6C, 0xCE, 0x98, 0x91, 0xB9, 0xB3, 0x11, 0x87,
    0x9E, 0x47, 0xF5, 0x67, 0x02, 0x00, 0x4B, 0xE5
};
BLEService        lbs(LED_SERVICE_UUID);
BLECharacteristic lsbLED(LED_CHR_UUID);
BLEConnection* connection;


bool isRunning = false;
// float motorSpeed = 1000.0; // кроків/сек
bool dir = true;
float targetSpeed = 1000.0; // кроків/сек
float acceleration = 1000.0; // кроків/сек^2


// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
    // Get the reference to current connection
    connection = Bluefruit.Connection(conn_handle);

    Serial.println("Connected to central");
    digitalWrite(LED, HIGH); //Set the LED to high
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Connected");
    display.display();
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
    Serial.println("Disconnected from central");
    digitalWrite(LED, LOW); //Set the LED to low
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Disconnected");
    display.display();
}


void setupAdv(void)
{

  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
 // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(lbs);

  // There isn't enough room in the advertising packet for the
  // name so we'll place it on the secondary Scan Response packet
  Bluefruit.ScanResponse.addName();
  // For recommended advertising interval
  // https://developer.apple.com/library/content/qa/qa1931/_index.html
  Bluefruit.Advertising.setInterval(320,360);    // in unit of 0.625 ms
  Bluefruit.Advertising.restartOnDisconnect(true);
}


void led_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len)
{
    static bool ledState = false;
  (void) conn_hdl;
  (void) chr;
  (void) len; // len should be 1

  // If least significant bit of data[0] is 0 then turn LED Off
  // If least significant bit of data[0] is 1 then turn LED On

    // digitalWrite(LED_BUILTIN, data[0]&1);
    if (!ledState) {
        ledState = true;
        digitalWrite(LED, HIGH); //Set the LED to high
    } else {
        ledState = false;
        digitalWrite(LED, LOW); //Set the LED to low
    }
    Serial.print("Data: ");
    Serial.println(data[0],HEX);
    lsbLED.write8(data[0]);

    switch (data[0])
    {
    case 30:
        // Start move stepper motor forward
        isRunning = true;
        digitalWrite(STEPPER_ENABLE_PIN, LOW); // Enable the stepper driver
        // motorSpeed = 1000.0; // кроків/сек
        stepper.setAcceleration(acceleration); // плавний розгін
        stepper.moveTo(1L << 30);  // дуже велике число (для CW)
        break;
    case 31:
        // Stop move stepper motor forward
        isRunning = false;
        stepper.setAcceleration(acceleration*5); // плавний розгін
        stepper.stop(); // миттєве гальмування
        break;

    case 40:
        // Start move stepper motor backward
        isRunning = true;
        digitalWrite(STEPPER_ENABLE_PIN, LOW); // Enable the stepper driver
        // motorSpeed = -1000.0; // кроків/сек
        stepper.setAcceleration(acceleration); // плавний розгін
        stepper.moveTo(-(1L << 30)); // для CCW
        break;
    case 41:
        // Stop move stepper motor backward
        isRunning = false;
        stepper.setAcceleration(acceleration*5); // плавний розгін
        stepper.stop(); // миттєве гальмування
        break;

    default:
        break;
    }


    // if(data[0] & 1) {
    //     stepper.step(1);
    // } else {
    //     stepper.step(-1);
    // }
}

void setup() {
    pinMode(LED, OUTPUT); //Set the LED to output mode.
    digitalWrite(LED, LOW);
    TinyUSBDevice.setManufacturerDescriptor("DROPLA");
    TinyUSBDevice.setProductDescriptor("robo_arm");
    Serial.begin(115200);

    pinMode(STEPPER_ENABLE_PIN, OUTPUT);
    digitalWrite(STEPPER_ENABLE_PIN, HIGH); // Disable the stepper driver initially
    // pinMode(STEPPER_RST_PIN, OUTPUT);
    // digitalWrite(STEPPER_RST_PIN, LOW); // Reset the stepper driver
    // delay(100); // wait for reset to complete
    // digitalWrite(STEPPER_RST_PIN, HIGH); // Release the reset
    // bluemicro_hid.begin();
    // stepper.setSpeed(1000);
    stepper.setMaxSpeed(targetSpeed); // максимум, який ви дозволяєте
    stepper.setAcceleration(acceleration); // плавний розгін
    // stepper.setSpeed(100); // за замовчуванням

    Bluefruit.begin();
    Bluefruit.setName("DROPLA-ROBO-ARM");
    // Configure callbacks
    Bluefruit.Periph.setConnectCallback(connect_callback);
    Bluefruit.Periph.setDisconnectCallback(disconnect_callback);


    lbs.begin();
    lsbLED.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE);
    lsbLED.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    lsbLED.setFixedLen(1);
    lsbLED.begin();
    lsbLED.write8(0x00); // led = off
    lsbLED.setWriteCallback(led_write_callback);

    delay(1000); // wait for Bluefruit to initialize

    #if 0
    bleuart.begin();

    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    Bluefruit.Advertising.addService(bleuart);
    Bluefruit.ScanResponse.addName();

    Bluefruit.Advertising.start();
    #endif

    // if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { //if error then builtin led light up
    //     digitalWrite(LED, HIGH); //Set the LED to high
    //     delay(10000);
    //     Serial.println("Error initializing display");
    //     while (true);
    // }
    // display.begin(SH1106_SWITCHCAPVCC, 0x3C);
    display.begin(0x3C, true); // Address 0x3C default
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE /*SSD1306_WHITE*/);
    display.setCursor(0, 0); //place where the text will be drawn
    display.println("By DROPLA");
    display.setCursor(0, 16);
    // display.println("Roboarm init...");

// Отримати BLE MAC-адресу через SoftDevice
  ble_gap_addr_t addr;
  if (sd_ble_gap_addr_get(&addr) == NRF_SUCCESS) {
    display.print("MAC Address: ");
    for (int i = 5; i >= 0; i--) {
      display.printf("%02X", addr.addr[i]);
      if (i > 0) display.print(":");
    }
    display.println();
  } else {
    display.println("Could not get MAC address");
  }

    display.display();

    // Set up Advertising Packet
    setupAdv();
    // Start Advertising
    Bluefruit.Advertising.start();

    delay(2000);
    Serial.println("Beginning ITG wheel remote sketch");
}

static int count = 0;

void loop() {
// Якщо прийшла команда з веб-додатку
//   if (bleuart.available()) {
//     String cmd = bleuart.readStringUntil('\n');
//     Serial.print("Command: "); Serial.println(cmd);

//     if (cmd == "ping") {
//       bleuart.println("pong");
//     } else {
//       bleuart.print("You said: ");
//       bleuart.println(cmd);
//     }
//   }

  // Для прикладу періодично надсилаємо дані
  static uint32_t lastSend = 0;
  if (millis() - lastSend > 1000) {
    // bleuart.println("Sensor data: 42");
    lastSend = millis();

    count++;
    // if(count % 2 == 0) {
    //   digitalWrite(LED, HIGH); //Set the LED to high
    // } else {
    //   digitalWrite(LED, LOW); //Set the LED to low
    // }
  Serial.print("Count: ");
  Serial.println(count);


    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(/*SSD1306_WHITE*/SH110X_WHITE);
    display.setCursor(0, 0); //place where the text will be drawn
    // Display the count value
    display.print("Count: ");
    display.print(count);
    display.display();
}
//   digitalWrite(LED, LOW); //Set the LED to low
//   delay(100); //wait 100ms

//   digitalWrite(LED, HIGH); //Set the LED to high
//   delay(100); //wait 100ms

//   delay(1000);
//   count++;
//   Serial.print("Count: ");
//   Serial.println(count);

//   // Print to Serial Monitor
//   Serial.println(" Beginning ITG wheel remote sketch");

//     display.clearDisplay();
//     display.setTextSize(2);
//     display.setTextColor(SSD1306_WHITE);
//     display.setCursor(0, 0); //place where the text will be drawn
//     // Display the count value
//     display.print("Count: ");
//     display.print(count);
//     display.display();

    // if(isRunning) {
    //     // If the stepper is running, set the speed and run it
    //     stepper.setSpeed(motorSpeed);
    //     stepper.runSpeed();
    // }

    if (isRunning || stepper.distanceToGo() != 0) {
        stepper.run(); // керує розгоном / гальмуванням
    } else {
        // Якщо кроковий двигун не рухається, вимикаємо драйвер
        #if !defined(USE_BREAK)
        digitalWrite(STEPPER_ENABLE_PIN, HIGH); // Disable the stepper driver
        #endif
    }


}

