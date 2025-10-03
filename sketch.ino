#define BLYNK_TEMPLATE_ID "TMPL3FkrdDnwB"
#define BLYNK_TEMPLATE_NAME "smart food expiry system"
#define BLYNK_AUTH_TOKEN "eDWsotEM9DlX2jC98sF0IHj48xNIQ4O3"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// ==== WiFi ====
char ssid[] = "Wokwi-GUEST";  
char pass[] = "";             

// ==== LCD ====
LiquidCrystal_I2C lcd(0x27, 16, 2);  

// ==== DHT22 ====
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ==== MQ2 (not used, just kept) ====
#define MQ2_PIN 34

// ==== Buzzer ====
#define BUZZER 2

// ====== Food Condition Database ======
struct FoodCondition {
  String name;
  int maxTemp;
  int maxHum;
};

FoodCondition foodDB[] = {
  {"Milk", 4, 95}, {"Apple", 4, 95},
  {"Banana", 15, 95}, {"Tomato", 15, 95},
  {"Potato", 10, 90}, {"Onion", 25, 75},
  {"Chicken", 2, 90}, {"Fish", 2, 95},
  {"Bread", 25, 70}, {"Rice", 25, 70}
};
int foodCount = sizeof(foodDB) / sizeof(foodDB[0]);

// ====== Food Items ======
struct FoodItem {
  String name;
  int daysLeft;
  String status;
  FoodCondition cond;
};
FoodItem items[10];
int itemCount = 0;

String currentFood = "";
int expiryDays = -1;

// ====== Logger ======
void logToTerminal(String msg) {
  Blynk.virtualWrite(V4, msg + "\n");  // append message line by line
  Serial.println(msg);                 // also print in serial monitor
}

// ====== Beep Functions ======
void beepOnce() {
  tone(BUZZER, 1000); delay(200); noTone(BUZZER);
}
void beepTwice() {
  for (int i = 0; i < 2; i++) {
    tone(BUZZER, 1000); delay(200); noTone(BUZZER); delay(200);
  }
}

// ====== Get Condition ======
FoodCondition getFoodCondition(String food) {
  for (int i = 0; i < foodCount; i++) {
    if (food.equalsIgnoreCase(foodDB[i].name)) {
      return foodDB[i];
    }
  }
  return {"Unknown", 30, 100}; // default
}

// ====== Add Item ======
void addItem() {
  if (itemCount < 10 && expiryDays > 0 && currentFood.length() > 0) {
    items[itemCount].name = currentFood;
    items[itemCount].daysLeft = expiryDays;
    items[itemCount].status = "Safe";
    items[itemCount].cond = getFoodCondition(currentFood);
    itemCount++;

    // Log and LCD
    logToTerminal(currentFood + " added: " + String(expiryDays) + "d left");
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(currentFood);
    lcd.setCursor(0, 1); lcd.print("Added: " + String(expiryDays) + "d");

    currentFood = "";
    expiryDays = -1;
    Blynk.virtualWrite(V1, "");
    Blynk.virtualWrite(V2, "");
  }
}

// ====== Check Items ======
void checkItems() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  for (int i = 0; i < itemCount; i++) {
    if (items[i].status == "Expired") continue; // skip already expired

    // Expiry countdown
    if (items[i].daysLeft > 0) {
      items[i].daysLeft--;
      logToTerminal(items[i].name + ": " + String(items[i].daysLeft) + "d left");
    }

    // Expired check
    if (items[i].daysLeft <= 0) {
      items[i].status = "Expired";
      logToTerminal(items[i].name + " expired!");
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print(items[i].name);
      lcd.setCursor(0, 1); lcd.print("Expired!");
      beepTwice();
      continue; // skip unsafe check after expired
    }

    // Unsafe check (only max temp & hum)
    if (temp > items[i].cond.maxTemp) {
      items[i].status = "Unsafe";
      logToTerminal(items[i].name + " unsafe! Reason: Due to High Temp use it");
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print(items[i].name);
      lcd.setCursor(0, 1); lcd.print("High Temp!");
      beepOnce();
    } 
    else if (hum > items[i].cond.maxHum) {
      items[i].status = "Unsafe";
      logToTerminal(items[i].name + " unsafe! Reason: Due to High Humidity use it");
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print(items[i].name);
      lcd.setCursor(0, 1); lcd.print("High Hum!");
      beepOnce();
    }
  }
}

// ====== BLYNK INPUTS ======
BLYNK_WRITE(V1) { currentFood = param.asString(); if (expiryDays > 0) addItem(); }
BLYNK_WRITE(V2) { expiryDays = param.asInt(); if (currentFood.length() > 0) addItem(); }

// ====== SETUP ======
void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  lcd.init(); lcd.backlight();
  dht.begin();
  pinMode(BUZZER, OUTPUT);

  lcd.setCursor(0, 0); lcd.print("Food Expiry Sys");
  lcd.setCursor(0, 1); lcd.print("Connecting...");
  Blynk.virtualWrite(V4, ""); // clear terminal at start
}

// ====== LOOP ======
void loop() {
  Blynk.run();

  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 5000) {  // check every 5s (fast demo)
    lastCheck = millis();
    checkItems();
  }
}
