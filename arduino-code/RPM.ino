// =====================================================
// RPM SENSOR
// =====================================================


// =====================================================
// SETUP RPM SENSOR
// =====================================================

void setupRPM()
{
  pinMode(RPM_PIN, INPUT);

  attachInterrupt(
    digitalPinToInterrupt(RPM_PIN),
    rpmInterrupt,
    CHANGE
  );
}


// =====================================================
// INTERRUPT
// =====================================================

void rpmInterrupt()
{
  rpmPulseCount++;
}


// =====================================================
// RPM MESSEN
// =====================================================

void updateRPMMeasurement()
{
  unsigned long currentTime = millis();

  if (currentTime - lastRPMMeasurement >= RPM_MEASUREMENT_INTERVAL)
  {
    noInterrupts();
    unsigned long pulses = rpmPulseCount;
    rpmPulseCount = 0;
    interrupts();

    float elapsedTime = (currentTime - lastRPMMeasurement) / 1000.0;
    lastRPMMeasurement = currentTime;

    if (pulses == 0)
    {
      measuredRPM = 0.0;
      return;
    }

    float pulsesPerSecond = pulses / elapsedTime;

    measuredRPM = (pulsesPerSecond * 60.0) / MOTOR_POLES;
  }
}