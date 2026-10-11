#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"
#include <Servo.h>


// =====================================================
// HARDWARE
// =====================================================

LiquidCrystal_I2C lcd(0x27, 16, 4);

HX711 scale;


// =====================================================
// BETRIEBSMODUS
// =====================================================

enum Mode
{
  MODE_SELECT,
  MANUAL_MODE,
  AUTO_MODE
};

Mode currentMode = MODE_SELECT;


// =====================================================
// ROTARY ENCODER
// =====================================================

#define ENCODER_DT  7
#define ENCODER_CLK 3
#define ENCODER_SW  4

int lastCLK;
int encoderPosition = 0;


// =====================================================
// HX711 / LOAD CELL
// =====================================================

#define DT  5
#define SCK 6

const float CALIBRATION_FORCE = 20.0;
const long CALIBRATION_VALUE  = 930000;

long zeroOffset;

float measuredForce = 0.0;


// =====================================================
// ESC
// =====================================================

#define ESC_PIN 9

Servo esc;


// =====================================================
// SPANNUNGSSENSOR
// =====================================================

#define VOLTAGE_PIN A3

// Spannungsteiler: 47 kOhm oben / 10 kOhm unten
// Faktor = (47000 + 10000) / 10000
const float VOLTAGE_DIVIDER_FACTOR = 5.7;

float measuredVoltage = NAN;


// =====================================================
// STROMSENSOR
// =====================================================

#define CURRENT_PIN A2

// ACS758LCB-050B-PFF-T: Empfindlichkeit ca. 40 mV/A
const float CURRENT_SENSITIVITY = 0.040;

// Ausgangsspannung bei 0 A
float currentZeroVoltage = 2.5; 

float measuredCurrent = NAN;


// =====================================================
// RPM SENSOR
// =====================================================

#define RPM_PIN 2

// AOS Supernova 2207 1980KV: Konfiguration 12N14P -> 14 Pole
const float MOTOR_POLES = 14.0;

volatile unsigned long rpmPulseCount = 0;

unsigned long lastRPMMeasurement = 0;

const unsigned long RPM_MEASUREMENT_INTERVAL = 100;

float measuredRPM = NAN;


// =====================================================
// LCD AKTUALISIERUNG
// =====================================================

unsigned long lastLCDUpdate = 0;

const unsigned long LCD_UPDATE_INTERVAL = 500;

int animationFrame = 0;


// =====================================================
// USB-DATENÜBERTRAGUNG
// =====================================================

unsigned long lastDataUpdate = 0;

// Messdaten standardmässig alle 100 ms senden
const unsigned long DATA_UPDATE_INTERVAL = 100;


// =====================================================
// CUSTOM CHARACTERS
// =====================================================

byte hourglass1[] =
{
  B11111,
  B11111,
  B01110,
  B00100,
  B01010,
  B10001,
  B11111,
  B00000
};

byte hourglass2[] =
{
  B11111,
  B10001,
  B01110,
  B00100,
  B01010,
  B11111,
  B11111,
  B00000
};

byte hourglass3[] =
{
  B11111,
  B10001,
  B01010,
  B00100,
  B01110,
  B11111,
  B11111,
  B00000
};


// =====================================================
// MANUAL MODE
// =====================================================

// 0 = PWM, 1 = START, 2 = RETURN
int manualSelection = 0;

bool pwmEditing = false;

int pwmValue = 1000;

const int PWM_MIN  = 1000;
const int PWM_MAX  = 2000;
const int PWM_STEP = 25;
const unsigned long PWM_AUTO_MAX = 1400;
const unsigned long PWM_AUTO_STEP = 50;

bool manualRunning      = false;
bool manualTestFinished = false;

unsigned long manualStartTime = 0;

const unsigned long MANUAL_TEST_DURATION = 10000;


// =====================================================
// AUTOMATISCHER MODUS
// =====================================================

int autoPWM = PWM_MIN;

unsigned long autoStepStartTime = 0;

const unsigned long AUTO_STEP_DURATION = 2000;


// =====================================================
// SETUP
// =====================================================

void setup()
{
                    Wire.begin();
                    Wire.setWireTimeout(3000, true); // Stoppt Hänger: 3ms Timeout + Bus-Reset bei EMV-Störung

  // ---------------------------------------------------
  // USB / SERIAL
  // ---------------------------------------------------

  Serial.begin(115200);
  Serial.println("Started");
  

  // ---------------------------------------------------
  // LCD CUSTOM CHARACTERS
  // ---------------------------------------------------

  Serial.println("characters1");
  lcd.createChar(0, hourglass1);
  lcd.createChar(1, hourglass2);
  lcd.createChar(2, hourglass3);
  Serial.println("characters2");


  // ---------------------------------------------------
  // HARDWARE INITIALISIERUNG
  // ---------------------------------------------------

  setupEncoder();
  setupLoadCell();
  setupRPM();

  pinMode(VOLTAGE_PIN, INPUT);
  pinMode(CURRENT_PIN, INPUT);

  setupDisplay();
  Serial.println("hardware");


  // ---------------------------------------------------
  // ESC INITIALISIEREN
  // ---------------------------------------------------

  esc.attach(ESC_PIN);
  esc.writeMicroseconds(PWM_MIN);

  delay(1000);
  Serial.println("esc init");


  // ---------------------------------------------------
  // STARTBILDSCHIRM & KALIBRIERUNG
  // ---------------------------------------------------

  showStartScreen();
              Serial.println("nowbutton");
  waitForButton();
  
  calibrateACSSensor();
             
              Serial.println("startscreen");

  // ---------------------------------------------------
  // MODUSAUSWAHL
  // ---------------------------------------------------

  lcd.clear();

  showModeSelection();
  Serial.println("Setup done");
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
  readEncoder();

  if (buttonPressed())
  {
    handleButton();
  }

  if (currentMode == MANUAL_MODE)
  {
    ManualMode();
  }
  else if (currentMode == AUTO_MODE)
  {
    AutoMode();
  }

  delay(10);
}