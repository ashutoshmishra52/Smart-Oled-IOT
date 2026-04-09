#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include "time.h"

const char* ssid1 = "Pixel";
const char* pass1 = "freekahai";

const char* ssid2 = "Kirmada";
const char* pass2 = "Internet52";

String botToken = "YOUR_BOT_TOKEN";

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define BUZZER_PIN 2

Adafruit_SSD1306 display(128, 64, &Wire, -1);

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;

int lastUpdateID = 0;
String lastText = "";

bool showTextMode = false;
unsigned long textTimer = 0;

bool showEye = false;
unsigned long eyeTimer = 0;

bool showBell = false;
unsigned long bellTimer = 0;
int bellFrame = 0;

float cachedTemp = 0;
unsigned long tempTimer = 0;
bool tempAlert = false;

bool buzzerActive = false;
unsigned long buzzerTimer = 0;

float eyeX = 0, eyeY = 0;
float targetX = 0, targetY = 0;
unsigned long holdTimer = 0;

void connectWiFi() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);

  WiFi.begin(ssid1, pass1);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 6000) delay(300);

  if (WiFi.status() == WL_CONNECTED) return;

  WiFi.disconnect(true);
  delay(1000);

  WiFi.begin(ssid2, pass2);

  start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 6000) delay(300);
}

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println("Booting...");
  display.display();
  delay(800);

  connectWiFi();

  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(120);
    digitalWrite(BUZZER_PIN, LOW);
    delay(120);
  }

  dht.begin();
  delay(1500);

  configTime(gmtOffset_sec, 0, ntpServer);
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) delay(300);

  eyeTimer = millis();
}

void loop() {
  showDisplay();

  static unsigned long lastCall = 0;
  if (millis() - lastCall > 1000) {
    checkTelegram();
    lastCall = millis();
  }

  if (!showTextMode && millis() - eyeTimer > 15000) {
    showEye = true;
    eyeTimer = millis();
  }

  if (buzzerActive && millis() - buzzerTimer > 3000) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerActive = false;
  }

  if (showBell && millis() - bellTimer > 3000) {
    showBell = false;
  }
}

void showDisplay() {
  if (millis() - tempTimer > 2000) {
    float t = dht.readTemperature();
    if (!isnan(t)) cachedTemp = t;
    tempTimer = millis();
  }

  if (cachedTemp >= 40 && !tempAlert) {
    digitalWrite(BUZZER_PIN, HIGH);
    buzzerActive = true;
    buzzerTimer = millis();
    tempAlert = true;
  }
  if (cachedTemp < 38) tempAlert = false;

  struct tm timeinfo;
  bool timeOk = getLocalTime(&timeinfo);

  display.clearDisplay();

  if (showBell) {
    drawBell();
    delay(100);
    return;
  }

  if (showEye) {
    drawEyesAdvanced();
    display.display();
    if (millis() - eyeTimer > 5000) showEye = false;
    delay(10);
    return;
  }

  if (showTextMode) {
    display.setTextSize(2);
    display.setCursor(10, 25);
    display.println(lastText);
    display.display();

    if (millis() - textTimer > 5000) showTextMode = false;
    return;
  }

  display.setTextSize(3);
  display.setCursor(0, 0);
  if (timeOk) display.printf("%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);

  display.setTextSize(2);
  display.setCursor(0, 35);
  display.printf("%.1fC", cachedTemp);

  display.setTextSize(1);
  display.setCursor(80, 40);
  if (timeOk) display.printf("%02d/%02d", timeinfo.tm_mday, timeinfo.tm_mon + 1);

  display.display();
}

void drawBell() {
  display.clearDisplay();

  int x = 64, y = 30;

  display.drawCircle(x, y - 8, 12, WHITE);
  display.drawRoundRect(x - 12, y - 8, 24, 18, 5, WHITE);
  display.fillCircle(x, y + 10, 3, WHITE);

  if (bellFrame % 2 == 0) {
    display.drawLine(x - 18, y - 10, x - 26, y - 18, WHITE);
    display.drawLine(x + 18, y - 10, x + 26, y - 18, WHITE);
  }

  display.display();
  bellFrame++;
}

void drawEyesAdvanced() {
  int lx = 40, rx = 88, cy = 32;

  if (millis() - holdTimer > random(700, 1500)) {
    targetX = random(-8, 8);
    targetY = random(-5, 5);
    holdTimer = millis();
  }

  eyeX += (targetX - eyeX) * 0.35;
  eyeY += (targetY - eyeY) * 0.35;

  eyeX = constrain(eyeX, -7, 7);
  eyeY = constrain(eyeY, -4, 4);

  display.fillRoundRect(lx - 16, cy - 12, 32, 24, 10, WHITE);
  display.fillRoundRect(rx - 16, cy - 12, 32, 24, 10, WHITE);

  display.fillCircle(lx + eyeX, cy + eyeY, 4, BLACK);
  display.fillCircle(rx + eyeX, cy + eyeY, 4, BLACK);
}

void checkTelegram() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  String url = "https://api.telegram.org/bot" + botToken +
               "/getUpdates?offset=" + String(lastUpdateID + 1);

  http.begin(url);
  int code = http.GET();

  if (code == 200) {
    DynamicJsonDocument doc(4096);
    deserializeJson(doc, http.getString());

    for (JsonObject obj : doc["result"].as<JsonArray>()) {
      int id = obj["update_id"];

      if (id > lastUpdateID) {
        lastUpdateID = id;

        String text = obj["message"]["text"] | "";
        String chat_id = String(obj["message"]["chat"]["id"].as<long>());

        if (text == "/start") sendMessage(chat_id, "Bot Ready");

        else if (text == "Get Temperature")
          sendMessage(chat_id, "Temp: " + String(cachedTemp) + " C");

        else if (text == "Eye") {
          showEye = true;
          eyeTimer = millis();
        }

        else if (text == "Buzzer") {
          digitalWrite(BUZZER_PIN, HIGH);
          buzzerActive = true;
          buzzerTimer = millis();

          showBell = true;
          bellTimer = millis();
        }

        else {
          lastText = text;
          showTextMode = true;
          textTimer = millis();
          showEye = false;
        }
      }
    }
  }
  http.end();
}

void sendMessage(String chat_id, String text) {
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + botToken + "/sendMessage";

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  String payload =
    "{\"chat_id\":\"" + chat_id + "\","
    "\"text\":\"" + text + "\","
    "\"reply_markup\":{\"keyboard\":["
    "[{\"text\":\"Get Temperature\"}],"
    "[{\"text\":\"Eye\"}],"
    "[{\"text\":\"Buzzer\"}]],\"resize_keyboard\":true}}";

  http.POST(payload);
  http.end();
}
