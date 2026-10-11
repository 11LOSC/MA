// =====================================================
// AUTOMATISCHER MODUS
// =====================================================


// =====================================================
// AUTOMATISCHEN TEST STARTEN
// =====================================================

void startAutomaticTest()
{
  autoPWM = PWM_MIN;

  autoStepStartTime = millis();
  lastDataUpdate    = millis();

  tareLoadCell();

  lcd.clear();

  esc.writeMicroseconds(autoPWM);

  measuredRPM     = NAN;
  measuredVoltage = NAN;
  measuredCurrent = NAN;

  sendTestStart(true);

  lastLCDUpdate = millis() - LCD_UPDATE_INTERVAL;

  showMeasurementScreen(true);
}


// =====================================================
// AUTOMATISCHER MODUS LOOP
// =====================================================

void AutoMode()
{
  updateForceMeasurement();
  updateRPMMeasurement();

  if (millis() - autoStepStartTime >= AUTO_STEP_DURATION)
  {
    autoStepStartTime = millis();
    autoPWM += PWM_AUTO_STEP;

    if (autoPWM > PWM_AUTO_MAX)
    {
      esc.writeMicroseconds(PWM_MIN);

      sendTestEnd();

      currentMode     = MODE_SELECT;
      encoderPosition = 0;

      lcd.clear();
      showModeSelection();

      return;
    }

    esc.writeMicroseconds(autoPWM);
  }

  // --- WICHTIG: Hier wird das normale Update durchgeführt ---
  updateData(true);

  // --- DIAGNOSE-TEST BEI JEDEM UPDATE DURCHFÜHREN ---
  //runDiagnosticTest();

  if (millis() - lastLCDUpdate >= LCD_UPDATE_INTERVAL)
  {
    lastLCDUpdate = millis();
    showMeasurementScreen(true);
  }
}


// =====================================================
// DIAGNOSE-FUNKTIONEN FÜR VCC UND RAW-ANALOGWERTE
// =====================================================

float readVcc() 
{
  #if defined(__AVR_ATmega328P__)
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined(__AVR_ATmega32u4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif

  delay(2); // Warten auf Stabilisierung der Referenz
  ADCSRA |= _BV(ADSC); // Wandlung starten
  while (bit_is_set(ADCSRA, ADSC)); // Warten bis fertig

  uint8_t low  = ADCL;
  uint8_t high = ADCH;
  long result = (high << 8) | low;

  float vcc = 1125300L / result; 
  return vcc / 1000.0; // Volt
}

void runDiagnosticTest() 
{
  float currentVcc = readVcc();

  int a2_raw = analogRead(A2); // ACS758 Stromsensor
  int a3_raw = analogRead(A3); // Spannungssensor

  // Umrechnung basierend auf gemessener VCC
  float voltageA2_pin = (a2_raw / 1023.0) * currentVcc;
  float voltageA3_pin = (a3_raw / 1023.0) * currentVcc;

  // Anpassung an deinen Spannungsteiler & Stromsensor
  float calculatedVoltage = voltageA3_pin * 5.7; 
  float zeroOffsetPin     = currentVcc / 2.0; 
  float calculatedCurrent = (voltageA2_pin - zeroOffsetPin) / 0.040;

  // Ausgabe auf dem Seriellen Monitor
  Serial.print("VCC: ");
  Serial.print(currentVcc, 2);
  Serial.print(" V | A2 raw: ");
  Serial.print(a2_raw);
  Serial.print(" | A3 raw: ");
  Serial.print(a3_raw);
  Serial.print(" | Calc Volt: ");
  Serial.print(calculatedVoltage, 2);
  Serial.print(" V | Calc Curr: ");
  Serial.print(calculatedCurrent, 2);
  Serial.println(" A");
}