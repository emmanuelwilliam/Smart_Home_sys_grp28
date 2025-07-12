#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
//Pin Definitions
#define DHTPIN 2
#define DHTTYPE DHT22
#define GAS_SENSOR A0
#define BUZZER 8

//Relay Control Pins
#define RELAY_GAS_VALVE 3     // IN1
#define RELAY_HEATER    4     // IN2
#define RELAY_TEMP_FAN  5     // IN3
#define RELAY_GAS_FAN   6     // IN4

//Sensor & LCD Setup
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Thresholds
const int GAS_THRESHOLD = 1500;    // Above this - gas fan on and gas valve closed
const float TEMP_LOW = 15.0;       // Below this - heater ON
const float TEMP_HIGH =30.0;      // Above this - temp fan ON

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();

  pinMode(BUZZER, OUTPUT);

  // Setting relay pins as outputs
  pinMode(RELAY_GAS_VALVE, OUTPUT);
  pinMode(RELAY_HEATER, OUTPUT);
  pinMode(RELAY_TEMP_FAN, OUTPUT);
  pinMode(RELAY_GAS_FAN, OUTPUT);

  // Initial state: all OFF (NB: relay module HIGH = OFF for active LOW modules)
  digitalWrite(RELAY_GAS_VALVE, LOW);   // Valve OPEN (active LOW)
  digitalWrite(RELAY_HEATER, HIGH);     // OFF
  digitalWrite(RELAY_TEMP_FAN, HIGH);   // OFF
  digitalWrite(RELAY_GAS_FAN, HIGH);    // OFF

  lcd.setCursor(0, 0);
  lcd.print(" SmartHome Sys ");
  delay(2000);
  lcd.clear();
}

void loop() {
  float temp = dht.readTemperature();
  int gasLevel = analogRead(GAS_SENSOR);

  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print(" C, Gas: ");
  Serial.println(gasLevel);

  //Display Temperature
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print((char)223);  // Degree symbol
  lcd.print("C     ");

  //GAS DETECTION LOGIC
  if (gasLevel > GAS_THRESHOLD) {
    lcd.setCursor(0, 1);
    lcd.print("Gas Detected! ");
    digitalWrite(BUZZER, HIGH);
    digitalWrite(RELAY_GAS_VALVE, HIGH); // CLOSE valve (HIGH = OFF for active LOW relay)
    digitalWrite(RELAY_GAS_FAN, LOW);    // ON gas fan
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Gas: Normal   ");
    digitalWrite(BUZZER, LOW);
    digitalWrite(RELAY_GAS_VALVE, LOW);  // OPEN valve (LOW = ON for active LOW relay)
    digitalWrite(RELAY_GAS_FAN, HIGH);   // OFF gas fan
  }

  //TEMPERATURE CONTROL LOGIC
  if (temp < TEMP_LOW) {
    digitalWrite(RELAY_HEATER, LOW);     // ON heater
  } else {
    digitalWrite(RELAY_HEATER, HIGH);    // OFF heater
  }

  if (temp > TEMP_HIGH) {
    digitalWrite(RELAY_TEMP_FAN, LOW);   // ON temp fan
  } else {
    digitalWrite(RELAY_TEMP_FAN, HIGH);  // OFF temp fan
  }

  delay(1000);
}
