// =====================================================
// ROTARY ENCODER
// =====================================================


// =====================================================
// SETUP ENCODER
// =====================================================

void setupEncoder()
{
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  lastCLK = digitalRead(ENCODER_CLK);
}


// =====================================================
// ENCODER AUSLESEN
// =====================================================

void readEncoder()
{
  int currentCLK = digitalRead(ENCODER_CLK);

  if (currentCLK != lastCLK && currentCLK == LOW)
  {
    int dtState = digitalRead(ENCODER_DT);
    int direction = (dtState != currentCLK) ? -1 : 1;

    // -------------------------------------------------
    // MODUSAUSWAHL
    // -------------------------------------------------
    if (currentMode == MODE_SELECT)
    {
      encoderPosition += direction;

      if (encoderPosition < 0) encoderPosition = 2;
      if (encoderPosition > 2) encoderPosition = 0;

      showModeSelection();
    }

    // -------------------------------------------------
    // MANUELLER MODUS
    // -------------------------------------------------
    else if (currentMode == MANUAL_MODE)
    {
      if (manualRunning)
      {
        lastCLK = currentCLK;
        return;
      }

      if (pwmEditing)
      {
        pwmValue += direction * PWM_STEP;

        if (pwmValue < PWM_MIN) pwmValue = PWM_MIN;
        if (pwmValue > PWM_MAX) pwmValue = PWM_MAX;

        showManualMode();
      }
      else
      {
        manualSelection += direction;

        if (manualSelection < 0) manualSelection = 2;
        if (manualSelection > 2) manualSelection = 0;

        showManualMode();
      }
    }

    // -------------------------------------------------
    // AUTOMATISCHER MODUS
    // -------------------------------------------------
    else if (currentMode == AUTO_MODE)
    {
      lastCLK = currentCLK;
      return;
    }
  }

  lastCLK = currentCLK;
}


// =====================================================
// BUTTON
// =====================================================

bool buttonPressed()
{
  if (digitalRead(ENCODER_SW) == LOW)
  {
    delay(30);

    if (digitalRead(ENCODER_SW) == LOW)
    {
      while (digitalRead(ENCODER_SW) == LOW)
      {
        delay(1);
      }
      return true;
    }
  }
  return false;
}


// =====================================================
// AUF BUTTON WARTEN
// =====================================================

void waitForButton()
{
  while (digitalRead(ENCODER_SW) == HIGH)
  {
    delay(10);
  }

  delay(30);

  while (digitalRead(ENCODER_SW) == LOW)
  {
    delay(1);
  }
}


// =====================================================
// BUTTON-AKTIONEN
// =====================================================

void handleButton()
{
  // ---------------------------------------------------
  // MODUSAUSWAHL
  // ---------------------------------------------------
  if (currentMode == MODE_SELECT)
  {
    if (encoderPosition == 0)
    {
      currentMode     = MANUAL_MODE;
      manualSelection = 0;
      pwmEditing      = false;
      manualRunning   = false;

      lcd.clear();
      showManualMode();
    }
    else if (encoderPosition == 1)
    {
      currentMode = AUTO_MODE;
      startAutomaticTest();
    }
    else if (encoderPosition == 2)
    {
      showStartScreen();
      waitForButton();

      lcd.clear();
      showModeSelection();
    }
  }

  // ---------------------------------------------------
  // MANUELLER MODUS
  // ---------------------------------------------------
  else if (currentMode == MANUAL_MODE)
  {
    if (manualRunning)
    {
      esc.writeMicroseconds(PWM_MIN);

      manualRunning      = false;
      manualTestFinished = true;

      updateVoltageMeasurement();
      updateCurrentMeasurement();

      showMeasurementScreen(false);
    }
    else if (manualTestFinished)
    {
      manualTestFinished = false;

      currentMode     = MODE_SELECT;
      encoderPosition = 0;

      lcd.clear();
      showModeSelection();
    }
    else if (pwmEditing)
    {
      pwmEditing = false;
      showManualMode();
    }
    else if (manualSelection == 0)
    {
      pwmEditing = true;
      showManualMode();
    }
    else if (manualSelection == 1)
    {
      startManualTest();
    }
    else if (manualSelection == 2)
    {
      currentMode     = MODE_SELECT;
      encoderPosition = 0;

      lcd.clear();
      showModeSelection();
    }
  }

  // ---------------------------------------------------
  // AUTOMATISCHER MODUS
  // ---------------------------------------------------
  else if (currentMode == AUTO_MODE)
  {
    // Motor sofort stoppen
    esc.writeMicroseconds(PWM_MIN);

    // Test-Ende an PC senden
    sendTestEnd();

    // Modus & Variablen zurücksetzen
    autoPWM         = PWM_MIN;
    currentMode     = MODE_SELECT;
    encoderPosition = 0;

    lcd.clear();
    showModeSelection();
  }
}