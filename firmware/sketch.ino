#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Definitions
#define VOLTAGE_POT_PIN 34  // Potentiometer 1 -> Simulates ZMPT101B Voltage Sensor
#define CURRENT_POT_PIN 35  // Potentiometer 2 -> Simulates SCT-013 Current Sensor
#define BUZZER_PIN      4   // Buzzer -> Triggered on Poor Power Factor (< 0.85)
#define ALERT_LED_PIN   15  // Red LED -> Triggered on Over-Voltage or Over-Current

// Calibration & Threshold Constants
#define MAX_VOLTAGE_RMS 260.0  // Scaled max AC Voltage (Volts)
#define MAX_CURRENT_RMS 30.0   // Scaled max AC Current (Amperes)
#define OVER_CURRENT_LIMIT 20.0 // Fault threshold in Amps
#define OVER_VOLTAGE_LIMIT 245.0// Fault threshold in Volts
#define MIN_POWER_FACTOR 0.85  // Low power factor alarm threshold

// Global Energy Tracking Variables
double totalEnergykWh = 0.0;
unsigned long lastSampleTime = 0;

void setup() {
  Serial.begin(115200);

  // Initialize Output Pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(ALERT_LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(ALERT_LED_PIN, LOW);

  // Initialize I2C OLED Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C is standard I2C address
    Serial.println(F("SSD1306 OLED allocation failed!"));
    for (;;); // Stop execution if display fails
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(10, 20);
  display.println(F("ESP32 ENERGY METER"));
  display.setCursor(20, 35);
  display.println(F("Initializing..."));
  display.display();
  delay(2000);

  lastSampleTime = millis();
}

void loop() {
  // 1. Read Analog Sensors (0 to 4095)
  int rawVoltage = analogRead(VOLTAGE_POT_PIN);
  int rawCurrent = analogRead(CURRENT_POT_PIN);

  // 2. Scale Raw ADC Readings to Simulated Electrical Quantities
  float vRMS = (rawVoltage / 4095.0) * MAX_VOLTAGE_RMS;
  float iRMS = (rawCurrent / 4095.0) * MAX_CURRENT_RMS;

  // 3. Dynamic Power Factor Simulation based on load current
  // In real life, PF varies with load inductive reactance.
  float powerFactor = 0.95;
  if (iRMS > 5.0) {
    powerFactor = 0.92 - ((iRMS / MAX_CURRENT_RMS) * 0.25); // Drops as load increases
  } else if (iRMS < 0.5) {
    powerFactor = 1.0; // Resistive/No load
  }

  // 4. Power Calculations
  float activePowerW = vRMS * iRMS * powerFactor;            // P = V * I * PF (Watts)
  float apparentPowerVA = vRMS * iRMS;                       // S = V * I (VA)
  float activePowerkW = activePowerW / 1000.0;               // kW

  // 5. Energy Accumulation (kWh)
  unsigned long currentTime = millis();
  float timeIntervalHours = (currentTime - lastSampleTime) / 3600000.0;
  totalEnergykWh += (activePowerkW * timeIntervalHours);
  lastSampleTime = currentTime;

  // 6. Protection & Alarm Logic
  bool isOverVoltage = (vRMS > OVER_VOLTAGE_LIMIT);
  bool isOverCurrent = (iRMS > OVER_CURRENT_LIMIT);
  bool isPoorPF = (powerFactor < MIN_POWER_FACTOR && iRMS > 1.0);

  // Red LED Indicator for Over-Voltage / Over-Current Faults
  if (isOverVoltage || isOverCurrent) {
    digitalWrite(ALERT_LED_PIN, HIGH);
  } else {
    digitalWrite(ALERT_LED_PIN, LOW);
  }

  // Buzzer Alarm for Poor Power Factor
  if (isPoorPF) {
    tone(BUZZER_PIN, 1000); // 1 kHz Alarm Tone
  } else {
    noTone(BUZZER_PIN);
  }

  // 7. Update OLED Display
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  // Row 1: Voltage & Current
  display.print(F("V: ")); display.print(vRMS, 1); display.print(F("V  "));
  display.print(F("I: ")); display.print(iRMS, 1); display.println(F("A"));

  // Row 2: Active Power
  display.setCursor(0, 16);
  display.print(F("P: ")); display.print(activePowerW, 1); display.println(F(" W"));

  // Row 3: Power Factor
  display.setCursor(0, 32);
  display.print(F("PF: ")); display.print(powerFactor, 2);
  if (isPoorPF) display.print(F(" [LOW!]"));
  display.println();

  // Row 4: Accumulated Energy
  display.setCursor(0, 48);
  display.print(F("Energy: ")); display.print(totalEnergykWh, 4); display.println(F(" kWh"));

  display.display();

  // 8. Serial Monitor / Serial Plotter Output (CSV Format for easy plotting)
  Serial.print("Voltage_V:"); Serial.print(vRMS);
  Serial.print(",Current_A:"); Serial.print(iRMS);
  Serial.print(",Power_W:"); Serial.print(activePowerW);
  Serial.print(",PowerFactor:"); Serial.println(powerFactor);

  delay(500); // Refresh rate 2Hz
}
