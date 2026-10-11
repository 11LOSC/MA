// =====================================================
// STROMSENSOR (ACS758)
// =====================================================

// Globale Variable für den adaptiven Filter
float filteredCurrent = 0.0;
bool filterInitialized = false;


// =====================================================
// STROMSENSOR KALIBRIEREN (IM STILLSTAND)
// =====================================================

void calibrateACSSensor()
{
  long rawSum = 0;
  float startVCC = readRealVCC(); 
  
  for (int i = 0; i < 50; i++)
  {
    rawSum += analogRead(CURRENT_PIN);
    delay(2);
  }
  
  float avgRaw = rawSum / 50.0;
  
  currentZeroVoltage = avgRaw * (startVCC / 1023.0);

  filteredCurrent = 0.0;
  filterInitialized = false;
}


// =====================================================
// STROM MESSEN (ADAPTIVER ECHTZEIT-FILTER)
// =====================================================

void updateCurrentMeasurement()
{
  float realVCC = readRealVCC(); 

  // 1. Schnelles Oversampling im Messmoment (64 Samples = ca. 7 ms)
  // Das frisst das hochfrequente PWM-Rauschen, erzeugt aber NULL Trägheit.
  const int OVERSAMPLING_COUNT = 64;
  long rawSum = 0;
  for (int i = 0; i < OVERSAMPLING_COUNT; i++)
  {
    rawSum += analogRead(CURRENT_PIN);
  }
  
  float rawValue = (float)rawSum / OVERSAMPLING_COUNT;
  float currentPinVoltage = rawValue * (realVCC / 1023.0);

  // Instantaner Rohstromwert
  float instantCurrent = (currentPinVoltage - currentZeroVoltage) / CURRENT_SENSITIVITY;

  // Erstinitialisierung beim Start
  if (!filterInitialized)
  {
    filteredCurrent = instantCurrent;
    filterInitialized = true;
  }

  // 2. Adaptives Alpha berechnen (Dynamische Dämpfung)
  float delta = abs(instantCurrent - filteredCurrent);
  float alpha;

  if (delta > 2.0) 
  {
    // Großes Gas-Geben / Sprung (> 2 Ampere Abweichung): 
    // Filter schaltet auf 100 % Durchlass -> Sofortige Reaktion in Echtzeit!
    alpha = 1.0; 
  } 
  else if (delta > 0.5) 
  {
    // Mittlere Änderung: Schnelle Nachführung
    alpha = 0.5; 
  } 
  else 
  {
    // Konstantfahrt (nur Rauschen < 0.5 A): 
    // Starke Glättung für ruhige Messwerte
    alpha = 0.15; 
  }

  // Exponentielle Filterformel
  filteredCurrent = (alpha * instantCurrent) + ((1.0 - alpha) * filteredCurrent);

  measuredCurrent = filteredCurrent;

  // Noise-Gate für den Leerlauf
  if (abs(measuredCurrent) < 0.15)
  {
    measuredCurrent = 0.0;
  }
}