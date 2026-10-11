// =====================================================
// DISPLAY / LCD
// =====================================================


// =====================================================
// SETUP DISPLAY
// =====================================================

void setupDisplay()
{
  lcd.init();
  lcd.backlight();
}


// =====================================================
// STARTBILDSCHIRM
// =====================================================

void showStartScreen()
{
             
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("MASTERPROPS");

  lcd.setCursor(0, 1);
  lcd.print("TEST STAND");

  lcd.setCursor(3, 3);
  lcd.print("press to start...");
  
}


// =====================================================
// MODUSAUSWAHL
// =====================================================

void showModeSelection()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("CHOOSE MODE:");

  lcd.setCursor(0, 1);
  lcd.print((encoderPosition == 0) ? "> MANUAL" : "  MANUAL");

  lcd.setCursor(0, 2);
  lcd.print((encoderPosition == 1) ? "> AUTOMATIC" : "  AUTOMATIC");

  lcd.setCursor(12, 3);
  lcd.print((encoderPosition == 2) ? "> RETURN" : "  RETURN");
}


// =====================================================
// MANUELLER MODUS MENÜ
// =====================================================

void showManualMode()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("MANUAL MODE");

  lcd.setCursor(0, 1);
  if (manualSelection == 0)
  {
    if (pwmEditing)
    {
      lcd.print("< PWM:");
      lcd.print(pwmValue);
      lcd.print(" >");
    }
    else
    {
      lcd.print("> PWM:");
      lcd.print(pwmValue);
    }
  }
  else
  {
    lcd.print("  PWM:");
    lcd.print(pwmValue);
  }

  lcd.setCursor(0, 2);
  lcd.print((manualSelection == 1) ? "> START" : "  START");

  lcd.setCursor(12, 3);
  lcd.print((manualSelection == 2) ? "> RETURN" : "  RETURN");
}


// =====================================================
// MESSÜBERSICHT
// =====================================================

void showMeasurementScreen(bool automaticMode)
{
  // Zeile 1: Modus-Anzeige & Animation
  lcd.setCursor(0, 0);
  lcd.print(automaticMode ? "AUTO_MODE__________" : "MANUAL_MODE________");

  lcd.setCursor(19, 0);
  lcd.write(byte(animationFrame));

  animationFrame++;
  if (animationFrame > 2)
  {
    animationFrame = 0;
  }

  // Zeile 2: RPM & PWM
  lcd.setCursor(0, 1);
  lcd.print("RPM:");
  if (isnan(measuredRPM))
  {
    lcd.print("NaN");
  }
  else
  {
    lcd.print(measuredRPM, 0);
  }

  lcd.setCursor(11, 1);
  lcd.print("PWM:");
  lcd.print(automaticMode ? autoPWM : pwmValue);

  // Zeile 3: Spannung & Strom
  lcd.setCursor(2, 2);
  lcd.print("V:");
  if (isnan(measuredVoltage))
  {
    lcd.print("NaN");
  }
  else
  {
    lcd.print(measuredVoltage, 2);
  }

  lcd.setCursor(13, 2);
  lcd.print("A:");
  if (isnan(measuredCurrent))
  {
    lcd.print("NaN");
  }
  else
  {
    lcd.print(measuredCurrent, 1);
  }

  // Zeile 4: Kraft & Menü
  lcd.setCursor(2, 3);
  lcd.print("N:");
  lcd.print(measuredForce, 2);

  lcd.setCursor(12, 3);
  lcd.print(manualRunning ? "> STOPP" : "> RETURN");
}