// =====================================================
// SPANNUNGSSENSOR / VCC-SPION
// =====================================================


// =====================================================
// HILFSFUNKTION: ECHTE VCC MESSEN
// =====================================================

float readRealVCC()
{
  #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    ADCSRB &= ~_BV(MUX5);
  #endif
  
  delay(2); 
  ADCSRA |= _BV(ADSC); 
  while (bit_is_set(ADCSRA, ADSC)); 
  
  float vcc = 1125300L / ADC;
  return vcc / 1000.0;
}


// =====================================================
// SPANNUNG MESSEN
// =====================================================

void updateVoltageMeasurement()
{
  float realVCC = readRealVCC(); 

  int rawValue = analogRead(VOLTAGE_PIN);
  float sensorVoltage = rawValue * (realVCC / 1023.0);

  // Unkalibrierter Wert
  float uncalibratedVoltage = sensorVoltage * VOLTAGE_DIVIDER_FACTOR;

  // Zwei-Punkt-Kalibrierung
  measuredVoltage = uncalibratedVoltage * 0.8607; //(uncalibratedVoltage * 2.8493) - 51.5956;

  // Sicherheitsnetz gegen Rückwärtsspeisung im Leerlauf
  if (measuredVoltage < 10.0)
  {
    measuredVoltage = 0.0;
  }
}