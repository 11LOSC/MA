// =====================================================
// MANUELLER MODUS
// =====================================================

// Dauer des sanften Anlaufs
const unsigned long PWM_RAMP_DURATION = 2000;  // 2 Sekunden

unsigned long pwmRampStartTime = 0;
bool pwmRamping = false;


// =====================================================
// MANUELLEN TEST STARTEN
// =====================================================

void startManualTest()
{
  tareLoadCell();

  lcd.clear();

  manualRunning      = true;
  manualTestFinished = false;

  // PWM-Rampe starten
  pwmRampStartTime = millis();
  pwmRamping = true;

  // ESC zunächst auf Minimum
  esc.writeMicroseconds(PWM_MIN);

  measuredRPM     = NAN;
  measuredVoltage = NAN;
  measuredCurrent = NAN;

  lastDataUpdate = millis();

  sendTestStart(false);

  lastLCDUpdate = millis() - LCD_UPDATE_INTERVAL;

  showMeasurementScreen(false);
}


// =====================================================
// MANUELLER MODUS LOOP
// =====================================================

void ManualMode()
{
  if (!manualRunning) return;


  // ===================================================
  // SANFTER PWM-ANLAUF
  // 1000 µs -> Ziel-PWM innerhalb von 2 Sekunden
  // ===================================================

  if (pwmRamping)
  {
    unsigned long elapsed = millis() - pwmRampStartTime;

    // Rampe abgeschlossen
    if (elapsed >= PWM_RAMP_DURATION)
    {
      esc.writeMicroseconds(pwmValue);

      pwmRamping = false;

      // Erst jetzt beginnt die eigentliche Testdauer
      manualStartTime = millis();
      lastDataUpdate  = millis();
    }

    // Rampe noch nicht abgeschlossen
    else
    {
      float progress = (float)elapsed / PWM_RAMP_DURATION;

      int currentPWM = PWM_MIN +
                       (pwmValue - PWM_MIN) * progress;

      esc.writeMicroseconds(currentPWM);
    }
  }


  // ===================================================
  // MESSWERTE AKTUALISIEREN
  // ===================================================

  updateForceMeasurement();
  updateRPMMeasurement();


  // ===================================================
  // TESTDAUER
  // ===================================================

  // Während der Rampe noch keine Testzeit ablaufen lassen
  if (!pwmRamping &&
      millis() - manualStartTime >= MANUAL_TEST_DURATION)
  {
    esc.writeMicroseconds(PWM_MIN);

    sendTestEnd();

    manualRunning      = false;
    manualTestFinished = true;

    updateVoltageMeasurement();
    updateCurrentMeasurement();

    showMeasurementScreen(false);

    return;
  }


  // ===================================================
  // DATEN AUFZEICHNEN
  // ===================================================

  updateData(false);


  // ===================================================
  // LCD AKTUALISIEREN
  // ===================================================

  if (millis() - lastLCDUpdate >= LCD_UPDATE_INTERVAL)
  {
    lastLCDUpdate = millis();
    showMeasurementScreen(false);
  }
}