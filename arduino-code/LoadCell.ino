// =====================================================
// HX711 / LOAD CELL
// =====================================================


// =====================================================
// SETUP LOAD CELL
// =====================================================

void setupLoadCell()
{
  scale.begin(DT, SCK);

  delay(750);

  if (!scale.is_ready())
  {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("HX711 ERROR");

    lcd.setCursor(0, 1);
    lcd.print("not found!");

    while (1)
    {
      delay(100);
    }
  }

  delay(500);

  zeroOffset = scale.read();
}


// =====================================================
// LOAD CELL TAREN
// =====================================================

void tareLoadCell()
{
  while (!scale.is_ready())
  {
    delay(1);
  }

  const int TARE_SAMPLES = 10;
  long sum = 0;

  for (int i = 0; i < TARE_SAMPLES; i++)
  {
    while (!scale.is_ready())
    {
      delay(1);
    }

    sum += scale.read();
    delay(10);
  }

  zeroOffset = sum / TARE_SAMPLES;
  measuredForce = 0.0;
}


// =====================================================
// KRAFTMESSUNG
// =====================================================

void updateForceMeasurement()
{
  if (scale.is_ready())
  {
    long rawValue = scale.read();
    long difference = rawValue - zeroOffset;

    measuredForce = ((float)difference / CALIBRATION_VALUE) * CALIBRATION_FORCE;
  }
}