#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // Address 0x27, 16 cols, 2 rows

// DHT11 setup
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Sensor pins
#define PH_PIN A0
#define TDS_PIN A1
#define TURBIDITY_PIN A2

// Timing
unsigned long lastUpdate = 0;
int displayIndex = 0;

void setup() {
  lcd.begin(16, 2);
  lcd.backlight();

  dht.begin();
  analogReference(DEFAULT); // 5V reference

  lcd.setCursor(0, 0);
  lcd.print("Aquatic Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Update every 3 seconds
  if (millis() - lastUpdate > 3000) {
    lastUpdate = millis();

    float phVoltage = analogRead(PH_PIN) * (5.0 / 1023.0);
    float phValue = 15.74 * phVoltage - 21.04;
    if (phValue < 0) phValue = 0;
    if (phValue > 14) phValue = 14;

    int tdsRaw = analogRead(TDS_PIN);
    float tdsVoltage = tdsRaw * (5.0 / 1023.0);
    float tdsValue = (133.42 * tdsVoltage * tdsVoltage * tdsVoltage
                     - 255.86 * tdsVoltage * tdsVoltage
                     + 857.39 * tdsVoltage) * 0.5; // Approximation

    int turbidityRaw = analogRead(TURBIDITY_PIN);
    float turbidityVoltage = turbidityRaw * (5.0 / 1023.0);
    float turbidityNTU = map(turbidityRaw, 0, 1023, 3000, 0); // Rough estimation

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    lcd.clear();

    switch (displayIndex) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("pH Level:");
        lcd.setCursor(0, 1);
        lcd.print(phValue, 2);
        break;

      case 1:
        lcd.setCursor(0, 0);
        lcd.print("TDS (ppm):");
        lcd.setCursor(0, 1);
        lcd.print(tdsValue, 0);
        break;

      case 2:
        lcd.setCursor(0, 0);
        lcd.print("Turbidity:");
        lcd.setCursor(0, 1);
        lcd.print(turbidityNTU);
        lcd.print(" NTU");
        break;

      case 3:
        lcd.setCursor(0, 0);
        lcd.print("Temp:");
        lcd.print(temperature, 1);
        lcd.print("C");

        lcd.setCursor(0, 1);
        lcd.print("Humid:");
        lcd.print(humidity, 1);
        lcd.print("%");
        break;
    }

    displayIndex++;
    if (displayIndex > 3) displayIndex = 0;
  }
}
