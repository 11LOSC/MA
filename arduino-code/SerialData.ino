// =====================================================
// USB-DATENÜBERTRAGUNG
// =====================================================


// =====================================================
// TEST START
// =====================================================

void sendTestStart(bool automaticMode)
{
  Serial.print("START,");
  Serial.println(automaticMode ? "AUTOMATIC" : "MANUAL");
}


// =====================================================
// MESSDATEN SENDEN
// =====================================================

void sendMeasurementData(bool automaticMode)
{
  Serial.print("DATA,");

  // Zeit
  Serial.print(millis());
  Serial.print(",");

  // PWM
  Serial.print(automaticMode ? autoPWM : pwmValue);
  Serial.print(",");

  // RPM
  if (isnan(measuredRPM))
  {
    Serial.print("NaN");
  }
  else
  {
    Serial.print(measuredRPM, 0);
  }
  Serial.print(",");

  // Spannung
  if (isnan(measuredVoltage))
  {
    Serial.print("NaN");
  }
  else
  {
    Serial.print(measuredVoltage, 2);
  }
  Serial.print(",");

  // Strom
  if (isnan(measuredCurrent))
  {
    Serial.print("NaN");
  }
  else
  {
    Serial.print(measuredCurrent, 2);
  }
  Serial.print(",");

  // Kraft / Schub
  Serial.print(measuredForce, 2);

  Serial.println();
}


// =====================================================
// TEST ENDE
// =====================================================

void sendTestEnd()
{
  Serial.println("END");
}


// =====================================================
// DATENAKTUALISIERUNG
// =====================================================

void updateData(bool automaticMode)
{
  if (millis() - lastDataUpdate >= DATA_UPDATE_INTERVAL)
  {
    lastDataUpdate = millis();

    updateRPMMeasurement();
    updateVoltageMeasurement();
    updateCurrentMeasurement();

    sendMeasurementData(automaticMode);
  }
}